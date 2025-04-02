文章标题：**RT-Thread RISC-V 部分周期测试需求文档**

- 作者: 汪辰
- 联系方式: <unicorn_wang@outlook.com>
- 原文地址: <https://github.com/plctlab/plct-rt-thread/blob/notes/0.notes/20241223-rtt-test-guide.md>。

文章大纲

<!-- TOC -->

- [1. 测试目的](#1-测试目的)
- [2. 测试周期](#2-测试周期)
- [3. 测试覆盖开发板](#3-测试覆盖开发板)
- [4. 测试用例说明](#4-测试用例说明)
- [5. 编写测试报告](#5-编写测试报告)

<!-- /TOC -->

# 1. 测试目的

针对我们关注的重点看护产品，周期性地采用当时最新的 master 版本运行测试，验证功能性是否正常。确保 master 的开发不会破坏现有产品的功能。

# 2. 测试周期

每周一次，具体时间建议固定下来，至于具体是周几，可以适时调整。

# 3. 测试覆盖开发板

- bsp/cvitek 的 Duo
- bsp/cvitek 的 Duo 256M
- bsp/cvitek 的 Duo S
- bsp/qemu-virt64-riscv

# 4. 测试用例说明

**注意：以下编译不同 bsp 的 RT-Thread 版本（标准版 or Smart 版本）时需要使用对应的编译工具链。具体参考对应 bsp 的 README 说明。**

| CASE 编号 | 测试开发板            | CASE 描述 |
|-----------|-----------------------|-----------|
|case 1     |bsp/cvitek 的 duo 256M |测试 RV64 小核 & RV64 大核：</br>编译小核 BSP (`c906_little`)：配置 Board Type 为 duo 256M（默认），小核为 RT standard（默认且只支持 RT standard），编译正常，生成 `fip.bin`。</br>编译大核 BSP (`cv18xx_risc-v`)：配置 Board Type 为 duo 256M（默认），大核为 RT Smart（默认），使能 SDH/RTC/DISKFS/lwext4，编译正常，生成 `boot.sd`。</br>上电后控制台（串口 UART1）显示 RT-Thread 标准版 logo 并进入 msh，运行正常。</br>上电后控制台（串口 UART0）显示 RT-Thread Smart logo，可以挂载 ext4 文件系统并进入 ash，运行正常。</br>具体使用说明参考 [`bsp/cvitek/README.md`][1]。|
|case 2     |bsp/cvitek 的 duo 256M |测试 RV64 小核 & AARCH64 大核：</br>编译小核 BSP：确保 case 1 已经编译成功小核 BSP，小核的 `rtthread.bin` 以及对应的 `fip.bin` 已经生成。</br>编译大核 BSP (`cv18xx_aarch64`)：配置 Board Type 为 duo 256M（默认），大核为 RT Smart，编译正常，更新上一步生成的 `fip.bin` 并新生成 `boot.sd`。</br>上电后控制台（串口 UART1）显示 RT-Thread 标准版 logo 并进入 msh，运行正常。</br>上电后控制台（串口 UART0）显示 RT-Thread Smart logo 并进入 msh，运行正常。</br>**注意：测试时通过短接物理引脚 35（Boot-Switch）和 GND 来切换到 ARM 核。** 具体使用说明参考 [`bsp/cvitek/cv18xx_aarch64/README.md`][3]。|
|case 3     |bsp/cvitek 的 duo      |测试 RV64 小核 & RV64 大核：</br>编译小核 BSP (`c906_little`)：配置 Board Type 为 duo，小核为 RT standard（默认且只支持 RT standard），编译正常，生成 `fip.bin`。</br>编译大核 BSP (`cv18xx_risc-v`)：配置 Board Type 为 duo，大核为 RT Smart（默认），使能 SDH/RTC/DISKFS/lwext4，编译正常，生成 `boot.sd`。</br>上电后控制台（串口 UART1）显示 RT-Thread 标准版 logo 并进入 msh，运行正常。</br>上电后控制台（串口 UART0）显示 RT-Thread Smart logo，可以挂载 ext4 文件系统并进入 ash，运行正常。</br>具体使用说明参考 [`bsp/cvitek/README.md`][1]。|
|case 4     |bsp/cvitek 的 duo S    |测试 RV64 小核 & RV64 大核：</br>编译小核 BSP (`c906_little`)：配置 Board Type 为 duo S，小核为 RT standard（默认且只支持 RT standard），**另外注意需要改变默认 UART1 配置**，编译正常, 生成 `fip.bin`。</br>编译大核 BSP (`cv18xx_risc-v`)：配置 Board Type 为 duo S，大核为 RT Smart（默认），使能 SDH/RTC/DISKFS/lwext4，编译正常，生成 `boot.sd`。</br>上电后控制台（串口 UART1）显示 RT-Thread 标准版 logo 并进入 msh，运行正常。</br>上电后控制台（串口 UART0）显示 RT-Thread Smart logo，可以挂载 ext4 文件系统并进入 ash，运行正常。</br>具体使用说明参考 [`bsp/cvitek/README.md`][1]。|
|case 5     |bsp/qemu-virt64-riscv  |测试 QEMU 运行 RT standard: </br>配置为 RT standard 模式下（默认），编译正常, 生成 `rtthread.bin`。</br>运行 `run.sh` 后控制台（qemu）显示 RT-Thread 标准版 logo 并进入 msh，运行正常。</br>具体使用说明参考 [`bsp/qemu-virt64-riscv/README_cn.md`][2]。|
|case 6     |bsp/qemu-virt64-riscv  |测试 QEMU 运行 RT smart: </br>配置为 RT smart 模式下并使能 lwext4，编译正常, 生成 `rtthread.bin`。</br>运行 `run.sh <path_to_ext4_image>` 后控制台（qemu）显示 RT-Thread Smart logo 并可以挂载 ext4 文件进入 ash，运行正常，执行 poweroff 可以退出 qemu。</br>具体使用说明参考 [`bsp/qemu-virt64-riscv/README_cn.md`][2]。|

# 5. 编写测试报告

针对每次测试结果，请提供测试报告，测试报告需要提供如下内容：

- 测试对应的 master 上的 commit ID

- 测试结果，建议以如下表格形式提供：

  | CASE #    | Result   | Description |
  |-----------|----------|-------------|
  | case 1    | PASS     |             |
  | case 2    | FAIL..   | #10161, #10165 |
  | ......    | ......   | ......      |

  其中，测试结果为 `PASS` 或者 `FAIL`。

  如果为 `PASS`，"测试描述" 则可以空着不填。

  如果某个 case 的测试结果为 `FAIL`，请在 "测试描述" 那一列给出失败的现象描述。方便起见，如果是一个已知的问题并有对应的 issue，可以直接填 issue#，如果是新发现的问题，可以直接提新的 issue 并给出 issue# 就好，issue# 之间用 "," 分隔。这样详细的错误描述可以直接体现在 issue 中。提 issue 的位置在 <https://github.com/RT-Thread/rt-thread/issues>。

[1]:https://github.com/RT-Thread/rt-thread/blob/master/bsp/cvitek/README.md
[2]:https://github.com/RT-Thread/rt-thread/blob/master/bsp/qemu-virt64-riscv/README_cn.md
[3]:https://github.com/RT-Thread/rt-thread/blob/master/bsp/cvitek/cv18xx_aarch64/README.md