RK3568 GPIO 引脚号查找笔记
一、背景知识
RK3568 的 GPIO 控制器分为多个 Bank（组），每个 Bank 包含 32 个引脚。
内核为每个物理引脚分配一个 全局唯一的整数编号，通过 /sys/class/gpio 或 libgpiod 操作时使用的就是这个编号。

二、Bank 与全局编号的对应关系
Bank 名称	基址 (Base)	全局编号范围
GPIO0	0	0 ~ 31
GPIO1	32	32 ~ 63
GPIO2	64	64 ~ 95
GPIO3	96	96 ~ 127
GPIO4	128	128 ~ 159
每个 Bank 内部的 32 个引脚又按 A/B/C/D 分组，每组 8 个引脚：

组名	组内偏移范围
A0~A7	0 ~ 7
B0~B7	8 ~ 15
C0~C7	16 ~ 23
D0~D7	24 ~ 31
三、计算公式
全局编号 = Bank基址 + 组内偏移

其中 组内偏移 根据引脚名中的字母和数字计算：

A0~A7 → 偏移 0~7

B0~B7 → 偏移 8~15

C0~C7 → 偏移 16~23

D0~D7 → 偏移 24~31

示例：引脚名为 GPIO3_D0

Bank = GPIO3 → 基址 = 96

D0 → 组内偏移 = 24

全局编号 = 96 + 24 = 120

四、实操步骤（从原理图到控制 LED）
步骤1：从原理图获得引脚名
原理图上通常会标注类似 GPIO3_D0、GPIO1_C5、GPIO0_B2 的名称。

步骤2：确定 Bank 和基址
根据上文对应表找到基址。

步骤3：计算组内偏移
根据字母和数字计算偏移：

若字母为 A，偏移 = 数字

若字母为 B，偏移 = 8 + 数字

若字母为 C，偏移 = 16 + 数字

若字母为 D，偏移 = 24 + 数字

例如：GPIO1_C5 → Bank=1 → 基址=32；C5 → 偏移=16+5=21；全局编号=32+21=53。

步骤4：验证该 GPIO 是否可用（未被系统占用）
在开发板上执行：

bash
cat /sys/kernel/debug/gpio
查找对应编号是否出现，以及是否被占用（例如标有 sysfs 或某个驱动名称）。如果该编号没有出现，可能该引脚被复用为其他功能（如 I2C、UART），需查阅设备树确认。

步骤5：使用 sysfs 或 libgpiod 测试
sysfs 方式（内核 legacy 接口）：

bash
# 假设全局编号为 120
echo 120 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio120/direction
echo 1 > /sys/class/gpio/gpio120/value   # 点亮 LED
echo 0 > /sys/class/gpio/gpio120/value   # 熄灭
libgpiod 方式（推荐）：

bash
# 需要安装 libgpiod 工具
gpioset 3 24=1   # 设置 GPIO3_D0 为高电平（3表示bank3，24表示组内偏移）
gpioset 3 24=0
五、常见问题
Q1：为什么我算出来的编号 export 时提示 “Device or resource busy”？
A：该 GPIO 已被内核或其他驱动占用。可以用 cat /sys/kernel/debug/gpio 确认，若被占用则不能直接通过 sysfs 操作。

Q2：为什么 export 成功，但写 value 时硬件没反应？
A：可能硬件设计是高电平有效/低电平有效，或者需要额外使能上拉/下拉。检查原理图确认 LED 的接法（阳极接 GPIO，阴极接地？还是相反？）。

Q3：如何快速查看所有 GPIO 编号与物理引脚的对应关系？
A：执行 cat /sys/kernel/debug/gpio，输出中会显示每个 Bank 的起始编号以及每个引脚的使用情况。

六、速查表（RK3568）
引脚名	全局编号	计算公式
GPIO0_A0	0	0 + 0
GPIO0_A7	7	0 + 7
GPIO0_B0	8	0 + 8
GPIO1_C0	48	32 + 16
GPIO1_C7	55	32 + 23
GPIO2_D3	91	64 + 27
GPIO3_D0	120	96 + 24
GPIO4_A0	128	128 + 0
七、附加提示
不同厂家的 RK3568 开发板，原理图上的引脚名可能省略 GPIO 前缀，如 3_D0 表示 GPIO3_D0。

如果设备树中将该引脚配置为其他功能（如 pinctrl 设置为 I2C），则无法作为普通 GPIO 使用，除非修改设备树重新编译。

对于新内核（5.10+），建议使用 libgpiod 替代 /sys/class/gpio。