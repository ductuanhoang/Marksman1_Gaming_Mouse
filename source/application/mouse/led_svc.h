#ifndef __LED_SVC_H__
#define __LED_SVC_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32_config.h"
#include "ht32.h"

/* Exported typedefs ---------------------------------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------------------------------------*/
void led_init(void);
void led_svc(void);

#ifdef __cplusplus
}
#endif

#endif /* __LED_SVC_H__ */
