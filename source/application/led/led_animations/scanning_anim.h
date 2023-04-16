#ifndef DISABLE_RGB_MATRIX_SCANNING
RGB_MATRIX_EFFECT(SCANNING)
#    ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

uint8_t scanning_anim_buff[] = {0,10,30,90,255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

bool SCANNING(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);
    printf("SCANNING\r\n");
    HSV     hsv   = rgb_matrix_config.hsv[0];
    static uint32_t tick = 0;
    uint8_t i = 0;
    static uint8_t k = 0;
    if(g_rgb_timer - tick > 50) {
        tick = g_rgb_timer;
        k ++;
        if (k > sizeof(scanning_anim_buff)) {
            k = 0;
            // Next color
            rgb_matrix_config.hsv[0].h += 16;
        }

        scanning_anim_buff[0] = scanning_anim_buff[sizeof(scanning_anim_buff) - 1];
        for (i = sizeof(scanning_anim_buff) - 1; i > 0; i--) {
            scanning_anim_buff[i]=scanning_anim_buff[i-1];
        }
    }

    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();
        hsv.v = scanning_anim_buff[g_led_config.point[i].y];
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
#endif      // DISABLE_RGB_MATRIX_SCANNING
