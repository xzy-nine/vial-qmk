#pragma once

/*
 * Geehy APM32F103CBT6, Cortex-M3, 128 KB flash.
 * STM32duino bootloader v1.0 occupies 0x08000000-0x08001FFF (8 KB).
 * Application region: 0x08002000-0x0801FFFF (120 KB).
 *
 * Matrix, features, encoder and WS2812 live in info.json.
 * Vial identity lives in keymaps/vial/config.h.
 */

#define MATRIX_ROWS 2
#define MATRIX_COLS 3

#define DEVICE_VER 0x0100

#define DEBOUNCE 5
