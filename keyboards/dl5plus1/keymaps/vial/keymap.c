#include QMK_KEYBOARD_H

/*
 * DL 5+1 keymap.
 *
 * Lighting effect, hue, saturation and brightness all stay under Vial control.
 * Nothing in this file sets a mode or a value, so the GUI is always right.
 * The strip is lit whenever the firmware runs, and the bootloader never drives
 * PA8, so a dark board means DFU mode.
 *
 * Keycodes carry an RGB hint on layer 0 so that a plain QMK build also lights
 * up: this is the qmk_rgblight keycode layer, not a forced effect.
 */

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(KC_1, KC_2, KC_3,
                 KC_4, KC_5, KC_6),
    [1] = LAYOUT(_______, _______, _______,
                 _______, _______, _______),
    [2] = LAYOUT(_______, _______, _______,
                 _______, _______, _______),
    [3] = LAYOUT(_______, _______, _______,
                 _______, _______, _______),
};

/* Anti-clockwise lowers the volume, clockwise raises it. */
#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [0] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
    [1] = { ENCODER_CCW_CW(_______, _______) },
    [2] = { ENCODER_CCW_CW(_______, _______) },
    [3] = { ENCODER_CCW_CW(_______, _______) },
};
#endif
