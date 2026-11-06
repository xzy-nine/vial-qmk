# DL 5+1 键盘 QMK 复刻配置

基于固件反编译与硬件实测。原厂固件备份见 [../../backup/dl5plus1](../../backup/dl5plus1)。

## 一、硬件

| 项 | 值 |
|---|---|
| 主控 | Geehy APM32F103CBT6，Cortex-M3，LQFP48，128KB Flash |
| 启动方式 | STM32duino bootloader v1.0，8KB，应用起点 `0x08002000` |
| 按键 | 5 个，直连 GPIO |
| 旋钮 | 1 个，正交编码器 + 按压 |
| 灯 | 5 颗 WS2812 |

## 二、引脚定义

| 功能 | 引脚 | LQFP48 |
|---|---|---|
| 按键 1 | PA2 | 12 |
| 按键 2 | PA5 | 14 |
| 按键 3 | PA6 | 15 |
| 按键 4 | PA7 | 16 |
| 旋钮按压 | PA9 | 29 |
| WS2812 数据 | PA8 | 28 |
| 编码器 A 相 | PB9 | 46 |
| 编码器 B 相 | PB8 | 45 |
| USB D− | PA11 | 32 |
| USB D+ | PA12 | 33 |

定义位置：`info.json` 的 `matrix_pins`、`encoder`、`ws2812` 字段。`config.h` 不再重复定义引脚。

## 三、反编译结果

### 关键函数

| 地址 | 功能 | 依据 |
|---|---|---|
| `0x08002F04` | `gpio_read_pin` | 硬编码 GPIOB 基址，读 IDR 取位 |
| `0x08002F1C` | `ws2812_send_byte` | 写 BSRR/BRR 的 `0x100` 位（PA8），位循环 |
| `0x08002F9C` | `ws2812_flush` | 5 灯循环，每灯 3 字节 |
| `0x08007C34` | `gpio_set_pin_mode` | 查模式表后写 CRL/CRH |
| `0x08005060` | 扫描主循环 | 时间片调度，`tbh` 跳转表 |
| `0x08008C84` | 编码器正交解码 | 读 A/B，状态机查表 |
| `0x08002160` | 复位入口 | `msr MSP/PSP/CONTROL` |

### 数据表

| 地址 | 内容 |
|---|---|
| `0x0800B0C0` | GPIO 模式表：`04` 浮空、`02` 推挽2M、`08` 输入上下拉、`00` 模拟、`03` 推挽50M、`07` 复用推挽 |
| `0x0800B428` | 编码器转换表：`00 FF 01 00 01 00 00 FF FF 00 00 01 00 01 FF 00` |
| `0x0800B414` | 按键引脚表：`0x40010809`、`0x40010802`、`0x40010805`、`0x40010806`、`0x40010807` |

### 内存布局

| 地址 | 用途 |
|---|---|
| `0x2000156C` | WS2812 RGB 缓冲，5×3=15 字节 |
| `0x200027AD` | 编码器状态数组 |
| `0x20002C78` | 编码器计数/时间戳 |

## 四、实现要点

### WS2812 必须用位操作驱动

原厂固件在 `0x08002F9C` 中把 `BASEPRI` 设为 `0x30`，在整个传输期间屏蔽中断，然后用 `BSRR`/`BRR` 单比特操作配合空循环延时输出时序。

这决定了必须使用 `WS2812_DRIVER = bitbang`，不能用硬件 SPI 或 PWM。

### 编码器是软件轮询

镜像中不存在 TIM2/TIM3/TIM4 的配置，也没有 AFIO_EXTICR 或 EXTI 寄存器的访问。编码器由扫描循环直接读取 PB8/PB9。

### 键位表不在 Flash 中

Vial 把键位存储在模拟 EEPROM 区，运行时加载。因此镜像中找不到静态键位表。

## 五、文件

| 文件 | 用途 |
|---|---|
| `info.json` | 引脚、布局、USB ID、编码器分辨率，引脚的唯一来源 |
| `config.h` | 按键极性、消抖、Vial UID |
| `rules.mk` | MCU、引导程序、功能开关 |
| `dl5plus1.h` | `LAYOUT` 宏 |
| `dl5plus1.c` | 默认键位 |
| `keymaps/vial/keymap.c` | Vial 键位 |

`config.h` 只放 `info.json` 无法表达的设置。引脚一律写在 `info.json`，避免两处定义冲突。

## 六、待调整项

| 项 | 说明 |
|---|---|
| `VIAL_KEYBOARD_UID` | 全 0 占位，需用 `util/vial_generate_keyboard_uid.py` 生成 |
| 编码器方向 | 反向时交换 `pin_a`/`pin_b` 或取消注释 `ENCODER_DIRECTION_FLIP` |
| 编码器分辨率 | `resolution: 4` 为推定值，需实测 |
| WS2812 灯序 | 灯珠顺序与索引对应关系需实测 |
| 按键物理坐标 | `info.json` 中为占位值 |
| 按键默认键值 | 当前 F13–F16 + 静音，按原厂功能替换 |

## 七、构建与烧录

```
git clone https://github.com/vial-kb/vial-qmk
cd vial-qmk
# 放入 keyboards/dl5plus1
qmk setup -b vial
qmk compile -kb dl5plus1 -km vial
```

工具链：`arm-none-eabi-gcc` 位于 `C:\Program Files (x86)\Arm GNU Toolchain arm-none-eabi\14.2 rel1`。

烧录：

```
dfu-util -d 1eaf:0003 -a 2 -D dl5plus1_vial.bin
```

写入完成后拔插 USB 一次。
