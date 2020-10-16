# RISC-V 64 QEMU-virt板级支持包说明

## 1. 简介

这份RT-Thread BSP是针对 QEMU RISC-V 64 的一份移植，提供了基本的用户态程序与外设支持。

本BSP适配QEMU 4.1.0版本QEMU virt机型。由于QEMU virt机型的具体外设地址分配、支持特性随QEMU版本号不同会有差异，故不建议将此BSP配合其他版本QEMU使用。


## 2. 编译说明

使用``scons``工具编译该板级支持包。如果编译正确无误，会产生``rtthread.elf``、``rtthread.bin``文件。运行``generate.sh``可自动将``rtthread.elf``文件与``bootblock``整合为``image``文件。``image``可作为uboot的payload而被加载。

注：推荐使用sifive版GNU GCC工具链编译本BSP，如使用发行版自带工具链可能因复杂的ELF文件结构导致createimage工具功能异常。


## 3. 支持情况

| 驱动 | 支持情况  |  备注  |
| ------ | ----  | :------:  |
| UART | 支持 | UART0 |
| SD卡 | 支持 | 大小可调 |
| MMU  | 支持 | sv39 |
| 网卡 | 部分 | 仅作为块设备 |

## 4. 运行说明

根据[环境搭建说明](https://github.com/chenguokai/rv_uboot)建立环境后，使用e1000.sh脚本启动，RT-Thread的启动信息及用户态输出会被打印在启动终端。网卡输入输出可使用cat命令写入eth0设备/从eth0设备读出内容触发读写并通过wireshark等软件验证。

\*注:使用e1000脚本启动时进入uboot后需要按任意键及时打断autoboot并输入以下命令加载RT-Thread:

```
virtio read 10200000 0 2
boot
```

首次编译生成image后其中不含有文件系统，RT-Thread启动时自动挂载根文件系统的程序执行会失败，但仍可以正常进入shell。进入shell后执行mkfs为sdcard设备创建文件系统后，重新启动QEMU，即可观察到根文件系统成功挂载。

每次使用QEMU启动脚本启动RT-Thread后，需要使用resume.sh将相关桥接删除，否则再次运行QEMU时会因无法创建同名桥接而报错启动失败。