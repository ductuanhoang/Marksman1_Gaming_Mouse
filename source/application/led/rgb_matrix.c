/* Copyright 2017 Jason Williams
 * Copyright 2017 Jack Humbert
 * Copyright 2018 Yiancar
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

#include "rgb_matrix.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "rgb_timer.h"
#include "lib8tion/lib8tion.h"
#include "mouse_data.h"

#include "eeprom.h"

#define EECONFIG_RGB_MATRIX_ADDR    0U

#ifndef RGB_MATRIX_CENTER
const point_t k_rgb_matrix_center = {112, 32};
#else
const point_t k_rgb_matrix_center = RGB_MATRIX_CENTER;
#endif

// Generic effect runners
//#include "rgb_matrix_runners/effect_runner_dx_dy_dist.h"
//#include "rgb_matrix_runners/effect_runner_dx_dy.h"
//#include "rgb_matrix_runners/effect_runner_i.h"
//#include "rgb_matrix_runners/effect_runner_sin_cos_i.h"
//#include "rgb_matrix_runners/effect_runner_reactive.h"
//#include "rgb_matrix_runners/effect_runner_reactive_splash.h"

// ------------------------------------------
// -----Begin rgb effect includes macros-----
#define RGB_MATRIX_EFFECT(name)
#define RGB_MATRIX_CUSTOM_EFFECT_IMPLS

#include "led_animations/rgb_matrix_effects.inc"
#ifdef RGB_MATRIX_CUSTOM_KB
#    include "rgb_matrix_kb.inc"
#endif
#ifdef RGB_MATRIX_CUSTOM_USER
#    include "rgb_matrix_user.inc"
#endif

#undef RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#undef RGB_MATRIX_EFFECT
// -----End rgb effect includes macros-------
// ------------------------------------------

#if defined(RGB_DISABLE_AFTER_TIMEOUT) && !defined(RGB_DISABLE_TIMEOUT)
#    define RGB_DISABLE_TIMEOUT (RGB_DISABLE_AFTER_TIMEOUT * 1200UL)
#endif

#ifndef RGB_DISABLE_TIMEOUT
#    define RGB_DISABLE_TIMEOUT 0
#endif

#ifndef RGB_DISABLE_WHEN_USB_SUSPENDED
#    define RGB_DISABLE_WHEN_USB_SUSPENDED false
#endif

#if !defined(RGB_MATRIX_MAXIMUM_BRIGHTNESS) || RGB_MATRIX_MAXIMUM_BRIGHTNESS > UINT8_MAX
#    undef RGB_MATRIX_MAXIMUM_BRIGHTNESS
#    define RGB_MATRIX_MAXIMUM_BRIGHTNESS UINT8_MAX
#endif

#if !defined(RGB_MATRIX_HUE_STEP)
#    define RGB_MATRIX_HUE_STEP 8
#endif

#if !defined(RGB_MATRIX_SAT_STEP)
#    define RGB_MATRIX_SAT_STEP 16
#endif

#if !defined(RGB_MATRIX_VAL_STEP)
#    define RGB_MATRIX_VAL_STEP 16
#endif

#if !defined(RGB_MATRIX_SPD_STEP)
#    define RGB_MATRIX_SPD_STEP 16
#endif

#if !defined(RGB_MATRIX_DEFAULT_MODE)
#   define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_SOLID_COLOR//RGB_MATRIX_SCANNING /* RGB_MATRIX_GRADIENT_UP_DOWN */ /* RGB_MATRIX_BREATHING */
#endif

#if !defined(RGB_MATRIX_DEFAULT_HUE)
#    define RGB_MATRIX_DEFAULT_HUE 0
#endif

#if !defined(RGB_MATRIX_DEFAULT_SAT)
#    define RGB_MATRIX_DEFAULT_SAT UINT8_MAX
#endif

#if !defined(RGB_MATRIX_DEFAULT_VAL)
#    define RGB_MATRIX_DEFAULT_VAL (RGB_MATRIX_MAXIMUM_BRIGHTNESS/4)
#endif

#if !defined(RGB_MATRIX_DEFAULT_SPD)
#    define RGB_MATRIX_DEFAULT_SPD (UINT8_MAX)
#endif

#if !defined(RGB_MATRIX_DEFAULT_DIRECTION)
#   define RGB_MATRIX_DEFAULT_DIRECTION (0U)    /* 0: Up->Down; 1: Down->Up */
#endif

// globals
bool         g_suspend_state = false;
rgb_config_t rgb_matrix_config;

uint32_t     g_rgb_timer;
#ifdef RGB_MATRIX_FRAMEBUFFER_EFFECTS
uint8_t g_rgb_frame_buffer[MATRIX_ROWS][MATRIX_COLS] = {{0}};
#endif  // RGB_MATRIX_FRAMEBUFFER_EFFECTS
#ifdef RGB_MATRIX_KEYREACTIVE_ENABLED
last_hit_t g_last_hit_tracker;
#endif  // RGB_MATRIX_KEYREACTIVE_ENABLED

// internals
static uint8_t         rgb_last_enable   = UINT8_MAX;
static uint8_t         rgb_last_effect   = UINT8_MAX;
static effect_params_t rgb_effect_params = {0, 0xFF};
static rgb_task_states rgb_task_state    = SYNCING;
#if RGB_DISABLE_TIMEOUT > 0
static uint32_t rgb_anykey_timer;
#endif  // RGB_DISABLE_TIMEOUT > 0

// double buffers
static uint32_t rgb_timer_buffer;
#ifdef RGB_MATRIX_KEYREACTIVE_ENABLED
static last_hit_t last_hit_buffer;
#endif  // RGB_MATRIX_KEYREACTIVE_ENABLED

led_config_t g_led_config = {
    {
        { 2, 2 },   // 0
        { 2, 1 },   // 1
        { 2, 0 },   // 2
        { 0, 0 },   // 3
        { 0, 0 },   // 4
        { 0, 1 },   // 5
        { 0, 2 },   // 6
        { 0, 3 },   // 7
        { 0, 4 },   // 8
        { 1, 4 },   // 9
        { 2, 4 },   // 10
        { 2, 3 }    // 11
    },
    {
        4, 4, 4, 4, 4, 4, 4, 4, 4, 8, 4, 4
    }
};

void eeconfig_read_rgb_matrix(void)
{
    eeprom_read_block(&rgb_matrix_config, EECONFIG_RGB_MATRIX_ADDR, sizeof(rgb_matrix_config));
}

void eeconfig_update_rgb_matrix(void)
{
    eeprom_write_block(&rgb_matrix_config, EECONFIG_RGB_MATRIX_ADDR, sizeof(rgb_matrix_config));
}

void eeconfig_update_rgb_matrix_default(void) {
    printf("eeconfig_update_rgb_matrix_default\n");
    rgb_matrix_config.enable    = 1;
    rgb_matrix_config.mode      = RGB_MATRIX_DEFAULT_MODE;
    rgb_matrix_config.hsv[0]    = (HSV){RGB_MATRIX_DEFAULT_HUE, RGB_MATRIX_DEFAULT_SAT, RGB_MATRIX_DEFAULT_VAL};
    rgb_matrix_config.speed     = RGB_MATRIX_DEFAULT_SPD;
    rgb_matrix_config.direction = RGB_MATRIX_DEFAULT_DIRECTION;
    rgb_matrix_config.mode = 0;
    rgb_matrix_config.dpi_level_xy = 2;
    rgb_matrix_config.dpi_level_x = 2;
    eeconfig_update_rgb_matrix();
}

void eeconfig_debug_rgb_matrix(void) {
    printf("rgb_matrix_config EEPROM\n");
    printf("rgb_matrix_config.enable = %d\n", rgb_matrix_config.enable);
    printf("rgb_matrix_config.mode = %d\n", rgb_matrix_config.mode);
    printf("rgb_matrix_config.speed = %d\n", rgb_matrix_config.speed);
    printf("rgb_matrix_config.direction = %d\n", rgb_matrix_config.direction);
    for (uint8_t i = 0; i < sizeof(rgb_matrix_config.hsv) / sizeof(HSV); i ++) {
        printf("rgb_matrix_config.hsv[%d].h = %d\n", i, rgb_matrix_config.hsv[i].h);
        printf("rgb_matrix_config.hsv[%d].s = %d\n", i, rgb_matrix_config.hsv[i].s);
        printf("rgb_matrix_config.hsv[%d].v = %d\n", i, rgb_matrix_config.hsv[i].v);
    }
    rgb_matrix_config.mode = 0;
    rgb_matrix_config.dpi_level_xy = 2;
    rgb_matrix_config.dpi_level_x = 2;
    printf("rgb_matrix_config.dpi_level = %d\n", rgb_matrix_config.dpi_level_xy);
    printf("rgb_matrix_config.dpi_level = %d\n", rgb_matrix_config.dpi_level_x);
}

void rgb_matrix_update_pwm_buffers(void) { rgb_matrix_driver.flush(); }

void rgb_matrix_set_color(int index, uint8_t red, uint8_t green, uint8_t blue) { rgb_matrix_driver.set_color(index, red, green, blue); }

void rgb_matrix_set_color_all(uint8_t red, uint8_t green, uint8_t blue) { rgb_matrix_driver.set_color_all(red, green, blue); }

#if 0
bool process_rgb_matrix(uint16_t keycode, keyrecord_t *record) {
#if RGB_DISABLE_TIMEOUT > 0
    if (record->event.pressed) {
        rgb_anykey_timer = 0;
    }
#endif  // RGB_DISABLE_TIMEOUT > 0

#ifdef RGB_MATRIX_KEYREACTIVE_ENABLED
    uint8_t led[LED_HITS_TO_REMEMBER];
    uint8_t led_count = 0;

#    if defined(RGB_MATRIX_KEYRELEASES)
    if (!record->event.pressed)
#    elif defined(RGB_MATRIX_KEYPRESSES)
    if (record->event.pressed)
#    endif  // defined(RGB_MATRIX_KEYRELEASES)
    {
        led_count = rgb_matrix_map_row_column_to_led(record->event.key.row, record->event.key.col, led);
    }

    if (last_hit_buffer.count + led_count > LED_HITS_TO_REMEMBER) {
        memcpy(&last_hit_buffer.x[0], &last_hit_buffer.x[led_count], LED_HITS_TO_REMEMBER - led_count);
        memcpy(&last_hit_buffer.y[0], &last_hit_buffer.y[led_count], LED_HITS_TO_REMEMBER - led_count);
        memcpy(&last_hit_buffer.tick[0], &last_hit_buffer.tick[led_count], (LED_HITS_TO_REMEMBER - led_count) * 2);  // 16 bit
        memcpy(&last_hit_buffer.index[0], &last_hit_buffer.index[led_count], LED_HITS_TO_REMEMBER - led_count);
        last_hit_buffer.count--;
    }

    for (uint8_t i = 0; i < led_count; i++) {
        uint8_t index                = last_hit_buffer.count;
        last_hit_buffer.x[index]     = g_led_config.point[led[i]].x;
        last_hit_buffer.y[index]     = g_led_config.point[led[i]].y;
        last_hit_buffer.index[index] = led[i];
        last_hit_buffer.tick[index]  = 0;
        last_hit_buffer.count++;
    }
#endif  // RGB_MATRIX_KEYREACTIVE_ENABLED

#if defined(RGB_MATRIX_FRAMEBUFFER_EFFECTS) && !defined(DISABLE_RGB_MATRIX_TYPING_HEATMAP)
    if (rgb_matrix_config.mode == RGB_MATRIX_TYPING_HEATMAP) {
        process_rgb_matrix_typing_heatmap(record);
    }
#endif  // defined(RGB_MATRIX_FRAMEBUFFER_EFFECTS) && !defined(DISABLE_RGB_MATRIX_TYPING_HEATMAP)

    return true;
}
#endif

void rgb_matrix_test(void) {
    // Mask out bits 4 and 5
    // Increase the factor to make the test animation slower (and reduce to make it faster)
    uint8_t factor = 10;
    switch ((g_rgb_timer & (3 << factor)) >> factor) {
        // rgb_matrix_set_color_all(20, 0, 0);
        case 0: {
            rgb_matrix_set_color_all(20, 0, 0);
            // rgb_matrix_set_color_all(0, 20, 0);
            break;
        }
        case 1: {
            rgb_matrix_set_color_all(0, 20, 0);
            break;
        }
        case 2: {
            rgb_matrix_set_color_all(0, 0, 20);
            // rgb_matrix_set_color_all(0, 20, 0);
            break;
        }
        case 3: {
            rgb_matrix_set_color_all(20, 20, 20);
            // rgb_matrix_set_color_all(0, 20, 0);
            break;
        }
    }
}

static bool rgb_matrix_none(effect_params_t *params) {
    if (!params->init) {
        return false;
    }

    rgb_matrix_set_color_all(0, 0, 0);
    return false;
}

static void rgb_task_timers(void) {
#if defined(RGB_MATRIX_KEYREACTIVE_ENABLED) || RGB_DISABLE_TIMEOUT > 0
    uint32_t deltaTime = timer_elapsed32(rgb_timer_buffer);
#endif  // defined(RGB_MATRIX_KEYREACTIVE_ENABLED) || RGB_DISABLE_TIMEOUT > 0
    rgb_timer_buffer = timer_read32();

    // Update double buffer timers
#if RGB_DISABLE_TIMEOUT > 0
    if (rgb_anykey_timer < UINT32_MAX) {
        if (UINT32_MAX - deltaTime < rgb_anykey_timer) {
            rgb_anykey_timer = UINT32_MAX;
        } else {
            rgb_anykey_timer += deltaTime;
        }
    }
#endif  // RGB_DISABLE_TIMEOUT > 0

    // Update double buffer last hit timers
#ifdef RGB_MATRIX_KEYREACTIVE_ENABLED
    uint8_t count = last_hit_buffer.count;
    for (uint8_t i = 0; i < count; ++i) {
        if (UINT16_MAX - deltaTime < last_hit_buffer.tick[i]) {
            last_hit_buffer.count--;
            continue;
        }
        last_hit_buffer.tick[i] += deltaTime;
    }
#endif  // RGB_MATRIX_KEYREACTIVE_ENABLED
}

static void rgb_task_sync(void) {
    // next task
    if (timer_elapsed32(g_rgb_timer) >= RGB_MATRIX_LED_FLUSH_LIMIT) rgb_task_state = STARTING;
}

static void rgb_task_start(void) {
    // reset iter
    rgb_effect_params.iter = 0;

    // update double buffers
    g_rgb_timer = rgb_timer_buffer;
#ifdef RGB_MATRIX_KEYREACTIVE_ENABLED
    g_last_hit_tracker = last_hit_buffer;
#endif  // RGB_MATRIX_KEYREACTIVE_ENABLED

    // next task
    rgb_task_state = RENDERING;
}

static void rgb_task_render(uint8_t effect) {
    bool rendering         = false;
    rgb_effect_params.init = (effect != rgb_last_effect) || (rgb_matrix_config.enable != rgb_last_enable);
    // each effect can opt to do calculations
    // and/or request PWM buffer updates.
    switch (effect) {
        case RGB_MATRIX_NONE:
            rendering = rgb_matrix_none(&rgb_effect_params);
            break;

// ---------------------------------------------
// -----Begin rgb effect switch case macros-----
#define RGB_MATRIX_EFFECT(name, ...)          \
    case RGB_MATRIX_##name:                   \
        rendering = name(&rgb_effect_params); \
        break;
#include "led_animations/rgb_matrix_effects.inc"
#undef RGB_MATRIX_EFFECT

#if defined(RGB_MATRIX_CUSTOM_KB) || defined(RGB_MATRIX_CUSTOM_USER)
#    define RGB_MATRIX_EFFECT(name, ...)          \
        case RGB_MATRIX_CUSTOM_##name:            \
            rendering = name(&rgb_effect_params); \
            break;
#    ifdef RGB_MATRIX_CUSTOM_KB
#        include "rgb_matrix_kb.inc"
#    endif
#    ifdef RGB_MATRIX_CUSTOM_USER
#        include "rgb_matrix_user.inc"
#    endif
#    undef RGB_MATRIX_EFFECT
#endif
            // -----End rgb effect switch case macros-------
            // ---------------------------------------------

        // Factory default magic value
        /* case UINT8_MAX: { */
        default: {
            rgb_matrix_test();
            rgb_task_state = FLUSHING;
        }
            return;
    }

    rgb_effect_params.iter++;

    // next task
    if (!rendering) {
        rgb_task_state = FLUSHING;
        if (!rgb_effect_params.init && effect == RGB_MATRIX_NONE) {
            // We only need to flush once if we are RGB_MATRIX_NONE
            rgb_task_state = SYNCING;
        }
    }
}

static void rgb_task_flush(uint8_t effect) {
    // update last trackers after the first full render so we can init over several frames
    rgb_last_effect = effect;
    rgb_last_enable = rgb_matrix_config.enable;

    // update pwm buffers
    rgb_matrix_update_pwm_buffers();

    // next task
    rgb_task_state = SYNCING;
}

void rgb_matrix_task(void) {
    rgb_task_timers();

    // Ideally we would also stop sending zeros to the LED driver PWM buffers
    // while suspended and just do a software shutdown. This is a cheap hack for now.
//    bool suspend_backlight =
//#if RGB_DISABLE_WHEN_USB_SUSPENDED == true
//        g_suspend_state ||
//#endif  // RGB_DISABLE_WHEN_USB_SUSPENDED == true
//#if RGB_DISABLE_TIMEOUT > 0
//        (rgb_anykey_timer > (uint32_t)RGB_DISABLE_TIMEOUT) ||
//#endif  // RGB_DISABLE_TIMEOUT > 0
//        false;

    // uint8_t effect = suspend_backlight || !rgb_matrix_config.enable ? 0 : rgb_matrix_config.mode;

    uint8_t effect = rgb_matrix_config.mode;
    switch (rgb_task_state) {
        case STARTING:
            rgb_task_start();
            break;
        case RENDERING:
            rgb_task_render(effect);
            break;
        case FLUSHING:
            rgb_task_flush(effect);
            break;
        case SYNCING:
            rgb_task_sync();
            break;
    }

#if 0
    if (!suspend_backlight) {
        rgb_matrix_indicators();
    }
#endif

}

void rgb_matrix_init(void) {
    rgb_matrix_driver.init();

#ifdef RGB_MATRIX_KEYREACTIVE_ENABLED
    g_last_hit_tracker.count = 0;
    for (uint8_t i = 0; i < LED_HITS_TO_REMEMBER; ++i) {
        g_last_hit_tracker.tick[i] = UINT16_MAX;
    }

    last_hit_buffer.count = 0;
    for (uint8_t i = 0; i < LED_HITS_TO_REMEMBER; ++i) {
        last_hit_buffer.tick[i] = UINT16_MAX;
    }
#endif  // RGB_MATRIX_KEYREACTIVE_ENABLED
#if 0
    rgb_matrix_config.enable = 1;
    rgb_matrix_config.mode   = RGB_MATRIX_STARTUP_MODE;
    rgb_matrix_config.hsv    = (HSV){RGB_MATRIX_STARTUP_HUE, RGB_MATRIX_STARTUP_SAT, RGB_MATRIX_STARTUP_VAL};
    rgb_matrix_config.speed  = RGB_MATRIX_STARTUP_SPD;
#endif

    eeprom_init();

    eeconfig_read_rgb_matrix();
    if (!rgb_matrix_config.mode) {
       printf("rgb_matrix_init_drivers rgb_matrix_config.mode = 0. Write default values to EEPROM.\n");
       eeconfig_update_rgb_matrix_default();
    }
    eeconfig_debug_rgb_matrix();  // display current eeprom values
}

void rgb_matrix_set_suspend_state(bool state) {
    if (RGB_DISABLE_WHEN_USB_SUSPENDED && state) {
        rgb_matrix_set_color_all(0, 0, 0);  // turn off all LEDs when suspending
    }
    g_suspend_state = state;
}

bool rgb_matrix_get_suspend_state(void) { return g_suspend_state; }


led_flags_t rgb_matrix_get_flags(void) { return rgb_effect_params.flags; }

void rgb_matrix_set_flags(led_flags_t flags) { rgb_effect_params.flags = flags; }
