文章标题：**RTT bsp/cvitek riscv 大小核 image 构建过程分析笔记**

- 作者: 汪辰
- 联系方式: <unicorn_wang@outlook.com>
- 原文地址: <https://github.com/plctlab/plct-rt-thread/blob/notes/0.notes/20241115-bsp-cvitek-rv-image-package-analysis.md>。

文章大纲

<!-- TOC -->

- [小核 906L](#小核-906l)
- [大核 906B](#大核-906b)
- [`check_bootloader`](#check_bootloader)
- [`get_build_board`](#get_build_board)
  - [`get_available_board`](#get_available_board)
  - [`prepare_env`](#prepare_env)
- [`do_build`](#do_build)
- [clean\_all](#clean_all)
  - [clean\_uboot](#clean_uboot)
- [build\_fsbl](#build_fsbl)
  - [opensbi](#opensbi)
  - [u-boot-build](#u-boot-build)
  - [memory-map](#memory-map)
  - [u-boot-dts](#u-boot-dts)
  - [总结](#总结)
  - [fsbl-build](#fsbl-build)
- [do\_combine](#do_combine)

<!-- /TOC -->

本文基于 RTT 主线 83a250f05f 做了一个快速笔记，简单总结了目前 `bsp/cviteck` 下 riscv 大小核的 `fip.bin` 和 `boot.sd` 的构建过程。

# 小核 906L

`bsp/cvitek/c906_little/rtconfig.py` -> `bsp/cvitek/combine-fip.sh bsp/cvitek/c906_little/ rtthread.bin`

其中
- `PROJECT_PATH`=`bsp/cvitek/c906_little/`
- `IMAGE_NAME`=`rtthread.bin`

`combine-fip.sh` 执行如下逻辑：
- `get_board_type` @ `board_env.sh`: 从 `.config` 中找出 `BOARD_TYPE`，`STORAGE_TYPE`，譬如 `BOARD_TYPE` = `"milkv-duo"`，`STORAGE_TYPE` = `"sd"`
- [`check_bootloader` @ `board_env.sh`](#check_bootloader)， git clone 一个叫做 `cvitek_bootloader` 的仓库
- [get_build_board ${BOARD_TYPE}](#get_build_board) 注意传入的参数 `${BOARD_TYPE}` 和 `bsp/cvitek/cvitek_bootloader/device` 下的子目录要对应。这个函数的产出是一个对应 board 类型的 `.config` 文件
- 查看是否已经做过 build
  - 如果没有，就 [do_build](#do_build)
  - 否则做 [do_combine](#do_combine)


简单总结一下 sdk 定位一个产品的路径

- 先从 RTT 的 bsp 的 `.config` 中找出 `BOARD_TYPE`，`STORAGE_TYPE`, 注意 `get_board_type` 函数中的 `BOARD_VALUE` 数组对应着 `cvitek_bootloader/device` 下的子目录名字。
- 根据 `BOARD_TYPE`，找到 `bsp/cvitek/cvitek_bootloader/device` 下的子目录，定位到对应的 `boardconfig.sh`，即 `MILKV_BOARD_CONFIG`
- 导入 `boardconfig.sh`, 注意其中的 `MV_BOARD_CPU` 和 `MV_BOARD_LINK`
- 根据上一步导入的 `MV_BOARD_CPU` 和 `MV_BOARD_LINK` 信息，可以在 `cvitek_bootloader/build/boards` 下找到对应的产品更详细的配置。mapping 的方式是先根据 MV_BOARD_CPU 找到 cvitek_bootloader/build/boards 下的 cv180x 还是 cv181x（`MV_BOARD_CPU` 和这两个目录的对应关系可以看 `cvitek_bootloader/build/boards/chip_list.json` 这个文件）。然后再根据 `MV_BOARD_LINK` 找到 cv180x/cv181x 目录下的具体的产品配置目录。

# 大核 906B

`bsp/cvitek/cv18xx_risc-v/rtconfig.py` -> `bsp/cvitek/mksdimg.sh bsp/cvitek/cv18xx_risc-v/ Image`

其中
- `PROJECT_PATH`=`bsp/cvitek/cv18xx_risc-v/`
- `IMAGE_NAME`=`Image` //这里的 Image 和小核的 rtthread.bin 是一回事

大核这边比较简单，核心逻辑就是：

```shell
lzma -c -9 -f -k ${PROJECT_PATH}/${IMAGE_NAME} > ${PROJECT_PATH}/dtb/${BOARD_TYPE}/Image.lzma

mkdir -p ${ROOT_PATH}/output/${BOARD_TYPE}
./mkimage -f ${PROJECT_PATH}/dtb/${BOARD_TYPE}/multi.its -r ${ROOT_PATH}/output/${BOARD_TYPE}/boot.${STORAGE_TYPE}
```

mkimage 就是和 u-boot 配套的制作下一级 kernel 的 打包工具。rtt 中针对不同的 board，按照 board 分类放在 `bsp/cvitek/cv18xx_risc-v/dtb` 目录下。

```shell
$ ls dtb -l
total 20
drwxrwxr-x 2 u u 4096  5月 23 08:30 milkv-duo
drwxrwxr-x 2 u u 4096  5月 23 10:28 milkv-duo256m
drwxrwxr-x 2 u u 4096  5月 23 08:30 milkv-duo256m-spinor
drwxrwxr-x 2 u u 4096  5月 23 08:30 milkv-duo-spinor
drwxrwxr-x 2 u u 4096  9月  4 16:08 milkv-duos-sd
```

每个目录下会有一份 dtb 和一个 `multi.its` 文件，`multi.its` 用于描述 mkimage 的打包规则

做完大核 的 image 后，我们会将 大核的 Image 用 lzma 压缩后放在 dtb 目录下，然后调用 mkimage 打包生成 `boot.sd`

FIXME：感觉目前方案有以下几个问题：
- mkimage 直接作为 bin 放在源码树仓库中，污染。这个 mkimage 可以 apt install 的。
- 构建时会在 dtb 目录下生成 `Image.lzma`, 看上去也不是很好的干净的做法。最好有一个 output 目录，存放所有中间产品

# `check_bootloader`

@ `bsp/cvitek/board_env.sh`

git clone 一个叫做 cvitek_bootloader（<https://gitee.com/flyingcys/cvitek_bootloader>/<https://github.com/flyingcys/cvitek_bootloader>） 的仓库，看了一下，这个仓库实际上是将 [milkv duo sdk](https://github.com/milkv-duo/duo-buildroot-sdk/) 这个仓库中的一部分目录拿出来自己维护的。摘出来的目录主要包括

- build
- device
- fsbl
- opensbi
- u-boot-2021.10

这么做的原因我猜是是因为 milkv duo sdk 太大，完整 14G，没有必要，的确需要简化。

FIXME，但这么做存在一些缺点，譬如上游的更新没法及时同步。可能需要再看看 sophgo 的 sophpi，或许可以分几步来做，第一步还是用 cvitek_bootloader

# `get_build_board`

@ `cvitek_bootloader/env.sh`

- [get_available_board](#get_available_board)
- 根据传入的 `$1` 进行处理。
  如果是 "lunch" 那么列出 board 的菜单进行选择
  否则 `$1` 就是 `${BOARD_TYPE}`，执行 `MILKV_BOARD=${1}` 就是将 `MILKV_BOARD=${BOARD_TYPE}`
- `MILKV_BOARD_CONFIG=device/${MILKV_BOARD}/boardconfig.sh`
- [prepare_env](#prepare_env)

## `get_available_board`

搜索 `bsp/cvitek/cvitek_bootloader/device` 下的子目录，排除 common 外的子目录名字存放到 `MILKV_BOARD_ARRAY` 中，形如：

```
MILKV_BOARD_ARRAY="milkv-duo milkv-duo-spinand milkv-duo-spinor ..."
```

每个子目录下一般会有两个文件
- `boardconfig.sh`
- `genimage.cfg` （这个可能没有）如果 `$STORAGE_TYPE` 为 "sd" 的就会有这个，估计是针对带 sd 卡的制作 sdcard image

`boardconfig.sh` 里定义了一些板子的环境变量：以 `bsp/cvitek/cvitek_bootloader/device/milkv-duo/boardconfig.sh` 为例

```
export MV_BOARD=milkv-duo
export MV_BOARD_CPU=cv1800b
export MV_VENDOR=milkv
export MV_BUILD_ENV=milkvsetup.sh
export MV_BOARD_LINK=cv1800b_milkv_duo_sd
```

特别注意 `MV_BOARD_LINK` 这个变量，后面根据这个变量会定位到 `bsp/cvitek/cvitek_bootloader/build/boards` 下的具体的产品配置目录。

## `prepare_env`

- `source ${MILKV_BOARD_CONFIG}` 假设我们当前编译的项目是 milkv duo, 那么就导入 `device/milkv-duo/boardconfig.sh`,
- `source build/${MV_BUILD_ENV}`, 导入 `build/milkvsetup.sh`，这里面会 `source build/common_functions.sh`, 这个里面定义了很多公共函数，包括下面要调用的 defconfig
- `defconfig ${MV_BOARD_LINK}`， 执行 `defconfig cv1800b_milkv_duo_sd`， 这个操作实际上会定位到 `build/boards/cv180x/cv1800b_milkv_duo_sd/cv1800b_milkv_duo_sd_defconfig`

  代码是 `_call_kconfig_script "${FUNCNAME[0]}" "${BUILD_PATH}/boards/${chip_arch}/${board}/${board}_defconfig"`
  这里 ${chip_arch} 是 cv180x，${board} 是 $MV_BOARD_LINK, 
  
  这个 `build/boards/cv180x/cv1800b_milkv_duo_sd/cv1800b_milkv_duo_sd_defconfig` 里都是一堆 CONFIG 配置选项。
  
  `_call_kconfig_script` 的作用是会去执行 `build/scripts/defconfig.py` 这个脚本，并把  `build/boards/cv180x/cv1800b_milkv_duo_sd/cv1800b_milkv_duo_sd_defconfig` 这个 CONFIG 文件传给它
  执行 defconfig 的结果，就是根据 `cv1800b_milkv_duo_sd_defconfig` 展开后生成一个 `.config` 文件。
 
- 如果 `${STORAGE_TYPE}` 是 sd 的，则读入 `MILKV_IMAGE_CONFIG=device/${MILKV_BOARD}/genimage.cfg`

 
# `do_build`

@ `cvitek_bootloader/env.sh`

- `get_toolchain` @ cvitek_bootloader/env.sh：拉 toolchain 的压缩包下来并解压，注意这个 toolchain 和 RTT 无关，应该只会涉及编译 opensbi，u-boot
- `source build/milkvsetup.sh` // 这里面定义了很多 build 函数
- [clean_all @ cvitek_bootloader/build/milkvsetup.sh](#clean_all)
- [build_fsbl](#build_fsbl)

# clean_all

注意 `bsp/cvitek/cvitek_bootloader/build/cvisetup.sh` 中也有一个 `clean_all`，但看上去这里用 `bsp/cvitek/cvitek_bootloader/build/milkvsetup.sh` 代替了它，所以我们要看 `bsp/cvitek/cvitek_bootloader/build/milkvsetup.sh` 里的代码。

```shell
function clean_all()
{
  clean_uboot
  # clean_kernel
  # clean_ramdisk
  # clean_osdrv
  # clean_middleware
}
```
## clean_uboot

```shell
function clean_uboot()
{(
  print_notice "Run ${FUNCNAME[0]}() function"
  _build_uboot_env
  cd "$BUILD_PATH" || return
  make u-boot-clean
)}
```

最后两句 `cd "$BUILD_PATH"` 和 `make u-boot-clean` 会进入 `bsp/cvitek/cvitek_bootloader/build` 读取并执行其下的 Makefile
注意对于当前项目，会 include `scripts/fip_v2.mk`

所以注意下面代码，在 `bsp/cvitek/cvitek_bootloader/build/scripts/fip_v2.mk` 中：
```makefile
ifeq ($(call qstrip,${CONFIG_ARCH}),riscv)
u-boot-clean: opensbi-clean
endif
u-boot-clean: fsbl-clean
```

以及下面代码，在 `bsp/cvitek/cvitek_bootloader/build/Makefile` 中
```makefile
u-boot-clean: export KBUILD_OUTPUT=${UBOOT_PATH}/${UBOOT_OUTPUT_FOLDER}
u-boot-clean:
	$(call print_target)
	${Q}$(MAKE) -j${NPROC} -C ${UBOOT_PATH} distclean
	${Q}rm -f ${OUTPUT_DIR}/fip.bin ${UBOOT_PATH}/${UBOOT_OUTPUT_FOLDER}/u-boot.bin.lzma
```

综合来看，就是 uboot 依赖于 opensbi 以及 fsbl，所以 clean u-boot 之前会先 clean opensbi 和 fsbl，然后再 clean u-boot

所以看 build log 如下：
```shell
Run clean_uboot() function 
  [TARGET] opensbi-clean 
......
  [TARGET] fsbl-clean 
......
  [TARGET] u-boot-clean 
......
```

# build_fsbl

@ cvitek_bootloader/build/milkvsetup.sh

```shell
function build_fsbl()
{(
  print_notice "Run ${FUNCNAME[0]}() function"
  _build_uboot_env
  _build_opensbi_env
  cd "$BUILD_PATH" || return
  make fsbl-build
)}
```


关于 `make fsbl-build`， 这个要看 `cvitek_bootloader/build/Makefile`

```makefile
ifeq (${CONFIG_FIP_V1},y)
include scripts/fip_v1.mk
else ifeq (${CONFIG_FIP_V2},y)
include scripts/fip_v2.mk
else
$(error no fip version)
endif
```

我们是 `FIP_V2`，原因要看 `cvitek_bootloader/build/Kconfig`， 对于 cv181x 和 cv180x 都是 `FIP_V2`

所以我们 会 include `scripts/fip_v2.mk`

在这个文件中定义的 fsbl-build

```makefile
ifeq ($(call qstrip,${CONFIG_ARCH}),riscv)
fsbl-build: opensbi
endif
ifeq (${CONFIG_ENABLE_FREERTOS},y)
fsbl-build: rtos
......
endif
......
fsbl-build: u-boot-build memory-map
......
```

以上说明，在构建上， fsbl 依赖于 opensbi/u-boot-build/memory-map，因为在各个产品的 defconfig 中，譬如 `bsp/cvitek/cvitek_bootloader/build/boards/cv180x/cv1800b_milkv_duo_sd/cv1800b_milkv_duo_sd_defconfig` 中 `CONFIG_ENABLE_FREERTOS` 是被注释掉了，所以这里 fsbl 不依赖于 rtos。

## opensbi

定义在 `bsp/cvitek/cvitek_bootloader/build/scripts/fip_v2.mk`

```makefile
opensbi: export CROSS_COMPILE=$(CONFIG_CROSS_COMPILE_SDK)
opensbi: u-boot-build
......
```

## u-boot-build

定义在 bsp/cvitek/cvitek_bootloader/build/Makefile 中, 依赖于 memory-map 和 u-boot-dts
```makefile
u-boot-build: memory-map
u-boot-build: u-boot-dts
u-boot-build: ${UBOOT_PATH}/${UBOOT_OUTPUT_FOLDER} ${UBOOT_CVIPART_DEP} ${UBOOT_OUTPUT_CONFIG_PATH}
......
```

## memory-map

定义在 `bsp/cvitek/cvitek_bootloader/build/scripts/mmap.mk` 中, 这个文件会被 `bsp/cvitek/cvitek_bootloader/build/Makefile` 包含

```makefile
.PHONY: memory-map
......
ifeq ($(wildcard ${BOARD_MMAP_PATH}),)
memory-map:
else
memory-map: ${CVI_BOARD_MEMMAP_H_PATH} ${CVI_BOARD_MEMMAP_CONF_PATH} ${CVI_BOARD_MEMMAP_LD_PATH}
endif
```

## u-boot-dts

定义在 `bsp/cvitek/cvitek_bootloader/build/Makefile`

不过看上去对于 RISC-V 不会做什么

## 总结

所以整个 build_fsbl 会依赖于以下组件

```shell
Run build_fsbl() function 
  [TARGET] /home/u/ws/duo/cvitek_bootloader/u-boot-2021.10/build/cv1812cp_milkv_duo256m_sd/.config 
  [TARGET] u-boot-dts 
  [TARGET] u-boot-build
  [TARGET] opensbi
```

## fsbl-build

定义在 `bsp/cvitek/cvitek_bootloader/build/scripts/fip_v2.mk`

核心是进入 `bsp/cvitek/cvitek_bootloader/fsbl`, 执行 makefile， 其中定义了 DEFAULT_GOAL 为 all

```makefile
all: fip bl2 blmacros

include ${MAKE_HELPERS_DIRECTORY}fip.mk
```

大量的规则定义在 bsp/cvitek/cvitek_bootloader/fsbl/make_helpers/fip.mk 下，最后的打包过程就是由这些定义的了

```shell
  [TARGET] fsbl-build 
  ...
  TARGET gen-chip-conf
  ...
  TARGET blmacros 
  TARGET blmacros-env 
  ...
  TARGET bl2 
  TARGET fip-all # 这里会生成 fip.bin
echo "  [GEN] fip.bin"
  [GEN] fip.bin
. /home/u/ws/duo/cvitek_bootloader/fsbl/build/cv1812cp_milkv_duo256m_sd/blmacros.env && \
./plat/cv181x/fiptool.py -v genfip \
	'/home/u/ws/duo/cvitek_bootloader/fsbl/build/cv1812cp_milkv_duo256m_sd/fip.bin' \
	--MONITOR_RUNADDR="${MONITOR_RUNADDR}" \
	--BLCP_2ND_RUNADDR="${BLCP_2ND_RUNADDR}" \
	--CHIP_CONF='/home/u/ws/duo/cvitek_bootloader/fsbl/build/cv1812cp_milkv_duo256m_sd/chip_conf.bin' \
	--NOR_INFO='FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF' \
	--NAND_INFO='00000000'\
	--BL2='/home/u/ws/duo/cvitek_bootloader/fsbl/build/cv1812cp_milkv_duo256m_sd/bl2.bin' \
	--BLCP_IMG_RUNADDR=0x05200200 \
	--BLCP_PARAM_LOADADDR=0 \
	--BLCP=test/empty.bin \
	--DDR_PARAM='test/cv181x/ddr_param.bin' \
	--BLCP_2ND='/home/u/ws/duo/rt-thread/bsp/cvitek/c906_little/rtthread.bin' \
	--MONITOR='../opensbi/build/platform/generic/firmware/fw_dynamic.bin' \
	--LOADER_2ND='/home/u/ws/duo/cvitek_bootloader/u-boot-2021.10/build/cv1812cp_milkv_duo256m_sd/u-boot-raw.bin' \
	--compress='lzma'
INFO:root:PROG: fiptool.py
DEBUG:root:  BL2='/home/u/ws/duo/cvitek_bootloader/fsbl/build/cv1812cp_milkv_duo256m_sd/bl2.bin'
DEBUG:root:  BL2_FILL=None
DEBUG:root:  BLCP='test/empty.bin'
DEBUG:root:  BLCP_2ND='/home/u/ws/duo/rt-thread/bsp/cvitek/c906_little/rtthread.bin'
DEBUG:root:  BLCP_2ND_RUNADDR=2413821952
DEBUG:root:  BLCP_IMG_RUNADDR=85983744
DEBUG:root:  BLCP_PARAM_LOADADDR=0
DEBUG:root:  BLOCK_SIZE=None
DEBUG:root:  CHIP_CONF='/home/u/ws/duo/cvitek_bootloader/fsbl/build/cv1812cp_milkv_duo256m_sd/chip_conf.bin'
DEBUG:root:  DDR_PARAM='test/cv181x/ddr_param.bin'
DEBUG:root:  LOADER_2ND='/home/u/ws/duo/cvitek_bootloader/u-boot-2021.10/build/cv1812cp_milkv_duo256m_sd/u-boot-raw.bin'
DEBUG:root:  MONITOR='../opensbi/build/platform/generic/firmware/fw_dynamic.bin'
DEBUG:root:  MONITOR_RUNADDR=2147483648
DEBUG:root:  NAND_INFO=b'\x00\x00\x00\x00'
DEBUG:root:  NOR_INFO=b'\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff'
DEBUG:root:  OLD_FIP=None
DEBUG:root:  compress='lzma'
DEBUG:root:  func=<function generate_fip at 0x76507d8f60e0>
DEBUG:root:  output='/home/u/ws/duo/cvitek_bootloader/fsbl/build/cv1812cp_milkv_duo256m_sd/fip.bin'
DEBUG:root:  subcmd='genfip'
DEBUG:root:  verbose=10
DEBUG:root:generate_fip:
DEBUG:root:add_nor_info:
DEBUG:root:add_nand_info:
DEBUG:root:add_chip_conf:
DEBUG:root:add_blcp:
DEBUG:root:add_bl2:
DEBUG:root:ddr_param=0x2000 bytes
DEBUG:root:blcp_2nd=0x110d8 bytes
DEBUG:root:monitor=0x1af88 bytes
DEBUG:root:loader_2nd=0x73ef7 bytes
DEBUG:root:make_fip1:
INFO:root:add BLCP (0x0)
INFO:root:add BL2 (0x8200)
DEBUG:root:len(body1_bin) is 33280
DEBUG:root:len(fip1_bin) is 37376
DEBUG:root:make_fip2:
DEBUG:root:pack_blcp_2nd:
DEBUG:root:pack_monitor:
DEBUG:root:pack_loader_2nd:
INFO:root:lzma loader_2nd=0x3738b bytes wo header
DEBUG:root:len(param2_bin) is 4096
INFO:root:generated fip_bin is 456704 bytes
INFO:root:print(param1):
[   <MAGIC1: a=0x0 s=0x8 c=0xa31304c425643 <class 'int'>>,
    <MAGIC2: a=0x8 s=0x4 c=0x000000 <class 'int'>>,
    <PARAM_CKSUM: a=0xc s=0x4 c=0xcafe7276 <class 'int'>>,
    <NAND_INFO: a=0x10 s=0x80 c=0x000000 <class 'int'>>,
    <NOR_INFO: a=0x90 s=0x24 c=0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff <class 'int'>>,
    <FIP_FLAGS: a=0xb4 s=0x8 c=0x000000 <class 'int'>>,
    <CHIP_CONF_SIZE: a=0xbc s=0x4 c=0x0002f8 <class 'int'>>,
    <BLCP_IMG_CKSUM: a=0xc0 s=0x4 c=0xcafe0000 <class 'int'>>,
    <BLCP_IMG_SIZE: a=0xc4 s=0x4 c=0x000000 <class 'int'>>,
    <BLCP_IMG_RUNADDR: a=0xc8 s=0x4 c=0x5200200 <class 'int'>>,
    <BLCP_PARAM_LOADADDR: a=0xcc s=0x4 c=0x000000 <class 'int'>>,
    <BLCP_PARAM_SIZE: a=0xd0 s=0x4 c=0x000000 <class 'int'>>,
    <BL2_IMG_CKSUM: a=0xd4 s=0x4 c=0xcafe389b <class 'int'>>,
    <BL2_IMG_SIZE: a=0xd8 s=0x4 c=0x008200 <class 'int'>>,
    <BLD_IMG_SIZE: a=0xdc s=0x4 c=0x000000 <class 'int'>>,
    <PARAM2_LOADADDR: a=0xe0 s=0x4 c=0x009200 <class 'int'>>,
    <RESERVED1: a=0xe4 s=0x4 c=0x000000 <class 'int'>>,
    <CHIP_CONF: a=0xe8 s=0x2f8 c=0c00000e010000a00c00000e020000a0... <class 'bytes'>>,
    <BL_EK: a=0x3e0 s=0x20 c=00000000000000000000000000000000... <class 'bytes'>>,
    <ROOT_PK: a=0x400 s=0x200 c=00000000000000000000000000000000... <class 'bytes'>>,
    <BL_PK: a=0x600 s=0x200 c=00000000000000000000000000000000... <class 'bytes'>>,
    <BL_PK_SIG: a=0x800 s=0x200 c=00000000000000000000000000000000... <class 'bytes'>>,
    <CHIP_CONF_SIG: a=0xa00 s=0x200 c=00000000000000000000000000000000... <class 'bytes'>>,
    <BL2_IMG_SIG: a=0xc00 s=0x200 c=00000000000000000000000000000000... <class 'bytes'>>,
    <BLCP_IMG_SIG: a=0xe00 s=0x200 c=00000000000000000000000000000000... <class 'bytes'>>]
INFO:root:print(param2):
[   <MAGIC1: a=0x0 s=0x8 c=0xa3230444c5643 <class 'int'>>,
    <PARAM2_CKSUM: a=0x8 s=0x4 c=0xcafe78a0 <class 'int'>>,
    <RESERVED1: a=0xc s=0x4 c=00000000 <class 'bytes'>>,
    <DDR_PARAM_CKSUM: a=0x10 s=0x4 c=0xcafe667f <class 'int'>>,
    <DDR_PARAM_LOADADDR: a=0x14 s=0x4 c=0x00a200 <class 'int'>>,
    <DDR_PARAM_SIZE: a=0x18 s=0x4 c=0x002000 <class 'int'>>,
    <DDR_PARAM_RESERVED: a=0x1c s=0x4 c=0x000000 <class 'int'>>,
    <BLCP_2ND_CKSUM: a=0x20 s=0x4 c=0xcafee087 <class 'int'>>,
    <BLCP_2ND_LOADADDR: a=0x24 s=0x4 c=0x00c200 <class 'int'>>,
    <BLCP_2ND_SIZE: a=0x28 s=0x4 c=0x011200 <class 'int'>>,
    <BLCP_2ND_RUNADDR: a=0x2c s=0x4 c=0x8fe00000 <class 'int'>>,
    <MONITOR_CKSUM: a=0x30 s=0x4 c=0xcafead84 <class 'int'>>,
    <MONITOR_LOADADDR: a=0x34 s=0x4 c=0x01d400 <class 'int'>>,
    <MONITOR_SIZE: a=0x38 s=0x4 c=0x01b000 <class 'int'>>,
    <MONITOR_RUNADDR: a=0x3c s=0x4 c=0x80000000 <class 'int'>>,
    <LOADER_2ND_RESERVED0: a=0x40 s=0x4 c=0x000000 <class 'int'>>,
    <LOADER_2ND_LOADADDR: a=0x44 s=0x4 c=0x038400 <class 'int'>>,
    <LOADER_2ND_RESERVED1: a=0x48 s=0x4 c=0x000000 <class 'int'>>,
    <LOADER_2ND_RESERVED2: a=0x4c s=0x4 c=0x000000 <class 'int'>>,
    <RESERVED_LAST: a=0x50 s=0xfb0 c=00000000000000000000000000000000... <class 'bytes'>>]
INFO:root:print(ldr_2nd_hdr):
[   <JUMP0: a=0x0 s=0x4 c=0x01a005 <class 'int'>>,
    <MAGIC: a=0x4 s=0x4 c=0x414d3342 <class 'int'>>,
    <CKSUM: a=0x8 s=0x4 c=0xcafe7399 <class 'int'>>,
    <SIZE: a=0xc s=0x4 c=0x037400 <class 'int'>>,
    <RUNADDR: a=0x10 s=0x8 c=0x80200000 <class 'int'>>,
    <RESERVED1: a=0x18 s=0x4 c=0xdeadbeec <class 'int'>>,
    <RESERVED2: a=0x1c s=0x4 c=0x01a011 <class 'int'>>]
echo "  [LS] " $(ls -l '/home/u/ws/duo/cvitek_bootloader/fsbl/build/cv1812cp_milkv_duo256m_sd/fip.bin')
  [LS]  -rw-rw-r-- 1 u u 456704 Nov 12 09:50 /home/u/ws/duo/cvitek_bootloader/fsbl/build/cv1812cp_milkv_duo256m_sd/fip.bin
make[1]: Leaving directory '/home/u/ws/duo/cvitek_bootloader/fsbl'
cp /home/u/ws/duo/cvitek_bootloader/fsbl/build/cv1812cp_milkv_duo256m_sd/fip.bin /home/u/ws/duo/cvitek_bootloader/install/soc_cv1812cp_milkv_duo256m_sd/
cp /home/u/ws/duo/cvitek_bootloader/fsbl/build/cv1812cp_milkv_duo256m_sd/fip.bin /home/u/ws/duo/cvitek_bootloader/install/soc_cv1812cp_milkv_duo256m_sd/fip_spl.bin
```

# do_combine

@ `bsp/cvitek/cvitek_bootloader/env.sh`

就是在 `do_build` 已经做过的情况下，直接更新一下 `fip.bin`。

`do_build` 包括了 `do_combine`。`do_combine` 应该是自己根据 make 脚本抽取出来单独写的，方便在已经编译过 fsbl/obensbi/u-boot 情况下直接更新 `fip.bin`。

