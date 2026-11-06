#include QMK_KEYBOARD_H

/*
 * DL 5+1 keymap.
 *
 * Lighting is owned by the host through LampArray; nothing here forces an
 * effect, colour or brightness. The bootloader never drives PA8, so a dark
 * board means DFU mode.
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

#ifdef RGB_MATRIX_ENABLE
/*
 * The WS2812 chain does not follow key order. Measured on the real board:
 *   LED 0 at key (0,1)   LED 1 at key (0,0)   LED 2 at key (1,2)
 *   LED 3 at key (1,1)   LED 4 at key (1,0)
 * Key (0,2) has no LED, so its matrix entry is NO_LED.
 * The 224x64 point grid below is QMK internal; physical positions come from
 * lamparray_get_lamp_impl further down.
 */
led_config_t g_led_config = {
    {
        /* Key matrix to LED index: row 0 then row 1 */
        {1, 0, NO_LED},
        {4, 3, 2},
    },
    {
        /* LED index to position on the QMK 224x64 grid */
        {112, 0}, {  0, 0}, {224, 64}, {112, 64}, {  0, 64},
    },
    {
        /* LED index to flag: all five are per-key backlight */
        4, 4, 4, 4, 4,
    }
};

#    ifdef LAMPARRAY_ENABLE
/*
 * Physical lamp positions in micrometers, origin at the top-left key centre.
 * Key pitch is 19.05 mm, so column centres sit at x = 0 / 19050 / 38100 and row
 * centres at y = 0 / 19050. Key (0,2) carries no LED because the encoder sits
 * there, which leaves the reported array as five lamps across two rows.
 */
void lamparray_get_lamp_impl(uint16_t lamp_id, lamparray_attributes_response_t* data) {
    static const int32_t lamp_x[RGB_MATRIX_LED_COUNT] = {19050, 0, 38100, 19050, 0};
    static const int32_t lamp_y[RGB_MATRIX_LED_COUNT] = {0, 0, 19050, 19050, 19050};

    if (lamp_id >= RGB_MATRIX_LED_COUNT) {
        return;
    }

    data->position.x = lamp_x[lamp_id];
    data->position.y = lamp_y[lamp_id];
    data->position.z = 0;
    data->purposes   = LAMP_PURPOSE_CONTROL;
}
#    endif
#endif
