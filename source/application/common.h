//
// Created by ductu on 1/8/2021.
//

#ifndef __COMMON_H__
#define __COMMON_H__

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
enum
{
    E_LOG_LVL_NONE,
    E_LOG_LVL_INFO,
    E_LOG_LVL_ERROR,
    E_LOG_LVL_WARNING,
    E_LOG_LVL_DEBUG,
    E_LOG_LVL_NEVER
};

/* Console color */
#define NO_COLOR "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"
/*Background colors*/
#define BG_KOLORS_BLK "\x1b[40m" // Black
#define BG_KOLORS_RED "\x1b[41m" // Red
#define BG_KOLORS_GRN "\x1b[42m" // Green
#define BG_KOLORS_YEL "\x1b[43m" // Yellow
#define BG_KOLORS_BLU "\x1b[44m" // Blue

#define LOG_SHOULD_I(level) (level <= LOG_BUILD_LEVEL && level <= E_LOG_LVL_DEBUG)
#define LOG(level, tag, ...)                                          \
    do                                                                \
    {                                                                 \
        if (LOG_SHOULD_I(level))                                      \
        {                                                             \
            printf("[%s] %s:%d: " NO_COLOR, tag, __func__, __LINE__); \
            printf(__VA_ARGS__);                                      \
            printf("\r\n");                                           \
        }                                                             \
    } while (0)

#define LOG_BUILD_LEVEL E_LOG_LVL_DEBUG

#define APP_LOGE(...) LOG(E_LOG_LVL_ERROR, KRED "ERROR" NO_COLOR, __VA_ARGS__)
#define APP_LOGI(...) LOG(E_LOG_LVL_INFO, KGRN "INFOR" NO_COLOR, __VA_ARGS__)
#define APP_LOGD(...) LOG(E_LOG_LVL_DEBUG, KYEL "DEBUG" NO_COLOR, __VA_ARGS__)
#define APP_LOGW(...) LOG(E_LOG_LVL_WARNING, BG_KOLORS_YEL "ALARM" NO_COLOR, __VA_ARGS__)

#define FIRMWARE_VERSION "1.0.0"
#define HARDWARE_VERSION "2.0"
#define PRODUCT_MODEL "ASH-WP27-V1.0"
#define PRODUCT_ID "zaz8ubcaatv7tars"

#endif //_COMMON_H
