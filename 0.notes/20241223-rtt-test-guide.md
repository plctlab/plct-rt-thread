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

| CASE 编号 | 测试开发板            | CASE 描述 |
|-----------|-----------------------|-----------|
|case 1     |bsp/cvitek 的 duo      |测试大核（默认 RT Smart）下，上电后可以挂载 ext4 文件系统，控制台（串口）显示正常。具体使用说明参考 [`bsp/cvitek/README.md`][1]。|
|case 2     |bsp/cvitek 的 duo      |测试小核（只支持 RT standard）下，上电后控制台（串口）显示正常。具体使用说明参考 [`bsp/cvitek/README.md`][1]。|
|case 3     |bsp/cvitek 的 duo 256M |测试大核（默认 RT Smart）下，上电后可以挂载 ext4 文件系统，控制台（串口）显示正常。具体使用说明参考 [`bsp/cvitek/README.md`][1]。|
|case 4     |bsp/cvitek 的 duo 256M |测试小核（只支持 RT standard）下，上电后控制台（串口）显示正常。具体使用说明参考 [`bsp/cvitek/README.md`][1]。|
|case 5     |bsp/cvitek 的 duo S    |测试大核（默认 RT Smart）下，上电后可以挂载 ext4 文件系统，控制台（串口）显示正常。具体使用说明参考 [`bsp/cvitek/README.md`][1]。|
|case 6     |bsp/cvitek 的 duo S    |测试小核（只支持 RT standard）下，上电后控制台（串口）显示正常。具体使用说明参考 [`bsp/cvitek/README.md`][1]。|
|case 7     |bsp/qemu-virt64-riscv  |测试配置为 RT standard 模式下（默认），上电后控制台（qemu）显示正常。具体使用说明参考 [`bsp/qemu-virt64-riscv/README_cn.md`][2]。|
|case 8     |bsp/qemu-virt64-riscv  |测试配置为 RT smart 模式下，上电后可以挂载 ext4 文件， 上电后控制台（qemu）显示正常。具体使用说明参考 [`bsp/qemu-virt64-riscv/README_cn.md`][2]。|

# 5. 编写测试报告

针对每次测试结果，请提供测试报告，测试报告需要提供如下内容：

- 测试对应的 master 上的 commit ID

- 测试结果，建议以如下表格形式提供：

  | CASE 编号 | 测试结果 | 测试描述 |
  |-----------|----------|----------|
  | case 1    | XXX      | XXX      |
  | case 2    | ......   | ......   |
  | ......    | ......   | ......   |

  其中，测试结果为 `PASS` 或者 `FAIL`。

  如果某个 case 的测试结果为 `FAIL`，请在 "测试描述" 那一列给出失败的现象描述；如果为 `PASS`，"测试描述" 则可以空着不填。

[1]:https://github.com/RT-Thread/rt-thread/blob/master/bsp/cvitek/README.md
[2]:https://github.com/RT-Thread/rt-thread/blob/master/bsp/qemu-virt64-riscv/README_cn.md