/* Copyright 2023 wuque
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H
#include "os_detection.h"
#include "rgb_matrix.h"
#include "via.h"
enum my_keycodes {
  CHANGE_OS = SAFE_RANGE-64,
  ENTER_CYCLE_ALL
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_BSPC,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS, KC_DEL,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,  KC_PGUP,
        KC_LSFT,          KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, KC_UP,   KC_PGDN,
        KC_LCTL, KC_LGUI, KC_LALT,                            KC_SPC,                    KC_RALT, MO(1),            KC_LEFT, KC_DOWN, KC_RGHT
    ),
    [1] = LAYOUT(
        KC_TILD,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  _______, _______,
        CHANGE_OS, RGB_TOG, RGB_VAI, ENTER_CYCLE_ALL, _______, _______, _______, _______, _______, _______, _______, KC_VOLD, KC_VOLU, _______, _______,
        _______, RGB_HUD, RGB_VAD, RGB_HUI, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______,          _______, _______, _______, _______, _______, _______, KC_MUTE, _______, _______, _______, _______, _______, QK_BOOT,
        _______, _______, _______,                            _______,                   _______, _______,          _______, _______, _______
    )
};

#ifdef OS_DETECTION_ENABLE
os_variant_t system_type;
os_variant_t LED_FLAG      = OS_UNSURE;
uint32_t led_off_timer = 0; // Timer to track when to turn off the LED

void keyboard_post_init_user() {
    if (dynamic_keymap_get_keycode(0, 4, 1) != KC_LGUI) {
        system_type = OS_MACOS;
    } else {
        system_type = OS_WINDOWS;
    }
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    if (LED_FLAG && timer_elapsed32(led_off_timer) < 1000) {
        if (LED_FLAG == OS_MACOS)
            rgb_matrix_set_color(0, 84, 255, 44);
        else {
            rgb_matrix_set_color(0, 71, 158, 255);
        }
        return false;
    } else if (LED_FLAG && timer_elapsed32(led_off_timer) >= 1000) {
        LED_FLAG = OS_UNSURE;
        return false;
    }
    return true;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case CHANGE_OS:
            if (record->event.pressed) {
                // Do something when pressed
                LED_FLAG      = system_type;
                led_off_timer = timer_read32();
            } else {
                if (system_type == OS_WINDOWS) {
                    dynamic_keymap_set_keycode(0, 4, 1, KC_LALT);
                    dynamic_keymap_set_keycode(0, 4, 2, KC_LGUI);
                    dynamic_keymap_set_keycode(0, 4, 10, KC_RGUI);
                    system_type = OS_MACOS;
                } else {
                    dynamic_keymap_set_keycode(0, 4, 1, KC_LGUI);
                    dynamic_keymap_set_keycode(0, 4, 2, KC_LALT);
                    dynamic_keymap_set_keycode(0, 4, 10, KC_RALT);
                    system_type = OS_WINDOWS;
                }
            }

            return true;
        case ENTER_CYCLE_ALL:
            if (record->event.pressed) {
            } else {
                uint8_t rgb_mode_temp = rgb_matrix_get_mode();
                rgb_mode_temp=rgb_mode_temp+1;
                if(rgb_mode_temp==4){
                    rgb_mode_temp = 0;
                }
                    rgb_matrix_enable_noeeprom();
                    rgb_matrix_mode_noeeprom(rgb_mode_temp);
                    via_qmk_rgb_matrix_save();
            }
            return true;
        default:
            return true;
    }
}
#endif
