#ifndef DISABLE_RGB_MATRIX_GRADIENT_UP_DOWN
RGB_MATRIX_EFFECT(GRADIENT_UP_DOWN)
#    ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

bool GRADIENT_UP_DOWN(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);
    printf("GRADIENT_UP_DOWN\r\n");
    HSV     hsv   = rgb_matrix_config.hsv[0];
    static uint32_t tick = 0;
    uint8_t scale = 0;
    if(g_rgb_timer - tick > 10) {
        tick = g_rgb_timer;
        scale = scale8(40, rgb_matrix_config.speed/4);
        rgb_matrix_config.hsv[0].h -= scale;
    }

    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();
        // The y range will be 0..64, map this to 0..4
        // Relies on hue being 8-bit and wrapping
        hsv.h   = rgb_matrix_config.hsv[0].h + (g_led_config.point[i].y * 55);
        RGB rgb = hsv_to_rgb(hsv);

        /* Set DPI led color */
        if (HAS_FLAGS(g_led_config.flags[i], LED_FLAG_INDICATOR)) {
            RGB dpi_color = dpi_backlight_color[curr_dpi_idx_xy % MAX_DPI_INDEX];
            rgb_matrix_set_color(i, dpi_color.r, dpi_color.g, dpi_color.b);
            continue;
        }
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
    return led_max < DRIVER_LED_TOTAL;
}

#    endif  // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#endif      // DISABLE_RGB_MATRIX_GRADIENT_UP_DOWN
