#pragma once

/*
 * UID taken from the stock firmware configuration (dl5+1 current config.vil),
 * so that an exported .vil from the factory firmware imports unchanged.
 */
#define VIAL_KEYBOARD_UID {0xB8, 0xA0, 0xD1, 0x53, 0xD5, 0x16, 0x3A, 0x97}

/* Unlock combo: first two keys of the top row, matrix (0,0) and (0,1). */
#define VIAL_UNLOCK_COMBO_ROWS {0, 0}
#define VIAL_UNLOCK_COMBO_COLS {0, 1}

/* Match the stock firmware entry counts so a stock .vil imports without loss. */
#define VIAL_TAP_DANCE_ENTRIES 32
#define VIAL_COMBO_ENTRIES 32
#define VIAL_KEY_OVERRIDE_ENTRIES 32
#define DYNAMIC_KEYMAP_LAYER_COUNT 4
#define DYNAMIC_KEYMAP_MACRO_COUNT 16

/* Five WS2812 LEDs, geometry defined in keymap.c. */
#define RGB_MATRIX_LED_COUNT 5
