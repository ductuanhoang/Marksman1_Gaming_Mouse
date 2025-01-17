#ifndef DISABLE_RGB_MATRIX_GRADIENT_LEFT_RIGHT
RGB_MATRIX_EFFECT(GRADIENT_LEFT_RIGHT)
#    ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

bool GRADIENT_LEFT_RIGHT(effect_params_t* params) {
    printf("GRADIENT_LEFT_RIGHT\r\n");
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    HSV     hsv   = rgb_matrix_config.hsv[0];
    uint8_t scale = scale8(64, rgb_matrix_config.speed);
    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();
        // The x range will be 0..224, map this to 0..7
        // Relies on hue being 8-bit and wrapping
        hsv.h   = rgb_matrix_config.hsv[0].h + (scale * g_led_config.point[i].x >> 5);
        RGB rgb = hsv_to_rgb(hsv);
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
    return led_max < DRIVER_LED_TOTAL;
}

#    endif  // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#endif      // DISABLE_RGB_MATRIX_GRADIENT_LEFT_RIGHT
