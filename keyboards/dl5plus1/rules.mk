# DL 5+1, Geehy APM32F103CBT6 (STM32F103 clone), STM32duino bootloader v1.0
# 5 keys + 1 encoder with push, 5x WS2812 on PA8
#
# Pins, features, encoder and WS2812 live in info.json.
# VIA/VIAL flags and Vial GUI feature switches live in keymaps/vial/rules.mk.

MCU = STM32F103
BOOTLOADER = stm32duino

BOOTMAGIC_ENABLE = no
TRI_LAYER_ENABLE = no
SEND_STRING_ENABLE = no
AUTO_SHIFT_ENABLE = no
CAPS_WORD_ENABLE = no
LAYER_LOCK_ENABLE = no

SPACE_CADET_ENABLE = no
GRAVE_ESC_ENABLE = no
MAGIC_ENABLE = no
