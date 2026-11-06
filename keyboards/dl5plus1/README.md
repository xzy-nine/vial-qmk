# DL 5+1 键盘 QMK 配置

基于原厂固件反编译与硬件实测。原厂固件备份见 [../../backup/dl5plus1](../../backup/dl5plus1)。

## 一、硬件

| 项 | 值 |
|---|---|
| 主控 | Geehy APM32F103CBT6，Cortex-M3，LQFP48，128KB Flash，20KB RAM |
| 启动方式 | STM32duino bootloader v1.0，8KB，应用起点 `0x08002000` |
| 按键 | 5 个 + 1 个旋钮按压，2×3 矩阵 |
| 旋钮 | 1 个，正交编码器 |
| 灯 | 5 颗 WS2812，PA8 位操作驱动 |

## 二、引脚定义

| 功能 | 引脚 | LQFP48 |
|---|---|---|
| 矩阵行 0 | PA9 | 29 |
| 矩阵行 1 | PA2 | 12 |
| 矩阵列 0 | PA5 | 14 |
| 矩阵列 1 | PA6 | 15 |
| 矩阵列 2 | PA7 | 16 |
| WS2812 数据 | PA8 | 28 |
| 编码器 A 相 | PB9 | 46 |
| 编码器 B 相 | PB8 | 45 |
| USB D− | PA11 | 32 |
| USB D+ | PA12 | 33 |

引脚只写在 `info.json` 的 `matrix_pins`、`encoder`、`ws2812` 字段，`config.h` 不重复定义。

原厂固件引脚表位于 `0x080087C4`，顺序为 `0x40010809`、`0x40010802`、`0x40010805`、`0x40010806`、`0x40010807`，即前两行为行线、后三为列线。

## 三、灯效

灯效由宿主经 HID LampArray 控制，固件不提供本地效果，也不启用 VialRGB。

灯链顺序为实测结果：

| LED | 键位 | x (µm) | y (µm) |
|---|---|---|---|
| 0 | (0,1) | 19050 | 0 |
| 1 | (0,0) | 0 | 0 |
| 2 | (1,2) | 38100 | 19050 |
| 3 | (1,1) | 19050 | 19050 |
| 4 | (1,0) | 0 | 19050 |

键位 (0,2) 无灯，`g_led_config` 中该位置为 `NO_LED`。

`g_led_config` 的 224×64 网格只是 QMK 内部坐标，不参与对外上报。物理位置由 `keymap.c` 中的 `lamparray_get_lamp_impl` 以微米为单位直接给出，键距 19.05mm。

## 四、反编译结果

### 关键函数

| 地址 | 功能 | 依据 |
|---|---|---|
| `0x08002F04` | `gpio_read_pin` | 硬编码 GPIO 基址，读 IDR 取位 |
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
| `0x0800B414` | 按键引脚表 |

### 内存布局

| 地址 | 用途 |
|---|---|
| `0x2000156C` | WS2812 RGB 缓冲，5×3=15 字节 |
| `0x200027AD` | 编码器状态数组 |
| `0x20002C78` | 编码器计数/时间戳 |

## 五、实现要点

### WS2812 用位操作驱动

原厂在传输期间把 `BASEPRI` 置为 `0x30` 屏蔽中断，再用 `BSRR`/`BRR` 单比特操作配合空循环延时输出时序。因此必须使用 `WS2812_DRIVER = bitbang`，不能用硬件 SPI 或 PWM。

### 编码器是软件轮询

镜像中不存在 TIM2/TIM3/TIM4 的配置，也没有 AFIO_EXTICR 或 EXTI 寄存器访问。编码器由扫描循环直接读取 PB8/PB9。

### 键位表不在 Flash 中

Vial 把键位存储在模拟 EEPROM 区，运行时加载，因此镜像中找不到静态键位表。

## 六、文件

| 文件 | 用途 |
|---|---|
| `info.json` | 引脚、布局、USB ID、功能开关，引脚的唯一来源 |
| `config.h` | 矩阵维度、`.vil` 条目数、灯数量 |
| `rules.mk` | MCU、引导程序、体积优化 |
| `dl5plus1.c` | 键盘级代码占位 |
| `keymaps/default/keymap.c` | 纯 QMK 键位，Vial 文档要求保留 |
| `keymaps/vial/config.h` | Vial UID、解锁组合、LampArray 尺寸 |
| `keymaps/vial/keymap.c` | 键位、编码器映射、灯链几何 |
| `keymaps/vial/rules.mk` | `VIA_ENABLE`、`VIAL_ENABLE`、`LAMPARRAY_ENABLE`、`ENCODER_MAP_ENABLE` |
| `keymaps/vial/vial.json` | 布局与编码器定义 |

## 七、qmk 源码修改

`LAMPARRAY_ENABLE` 依赖以下改动，重新克隆 qmk 后需重新应用：

| 文件 | 改动 |
|---|---|
| `builddefs/generic_features.mk` | `LAMPARRAY` 加入特性列表 |
| `builddefs/common_features.mk` | 注册 `lamparray.c` 与 `lamparray_usb.c` |
| `lib/python/qmk/math.py` | `ast.Num` 改为 `ast.Constant`，Python 3.12+ 必需 |
| `util/build_id.py` | `BUILD_ID` 改为确定性生成，避免刷固件清空配置 |
| `quantum/keyboard.c` | 调用 `lamparray_init` 与 `lamparray_task` |
| `quantum/quantum.h` | 包含 `lamparray.h` |
| `quantum/lamparray/` | lamparray 核心与 USB 回调 |
| `quantum/rgb_matrix/overlay.c` | 灯效覆盖层 |
| `tmk_core/protocol/usb_descriptor.c/.h` | LampArray 描述符、接口、端点 |
| `tmk_core/protocol/chibios/usb_endpoints.c/.h` | 端点注册 |
| `tmk_core/protocol/chibios/usb_main.c` | `GetReport` 失败时继续派发到端点回调 |

`usb_main.c` 的改动不可省略：原实现中 `HID_REQ_GetReport` 直接 `return usb_get_report_cb(usbp)`，通用处理返回 false 时请求即失败，LampArray 属性读不到，Windows 不会在动态照明中列出设备。

## 八、构建与烧录

工具链：`arm-none-eabi-gcc` 位于 `C:\Program Files (x86)\Arm GNU Toolchain arm-none-eabi\14.2 rel1`。

```
git clone https://github.com/vial-kb/vial-qmk
cd vial-qmk
make git-submodule
# 放入 keyboards/dl5plus1，应用第七节的 qmk 源码修改
make dl5plus1:vial
```

烧录：

```
dfu-util -d 1eaf:0003 -a 2 -D dl5plus1_vial.bin
```

写入完成后拔插 USB 一次。不要加 `-R`，该参数在本环境会触发 MANIFEST 阶段失败。

进入引导模式：按住第 1 键插 USB。灯全灭即引导模式，灯亮即固件运行。
