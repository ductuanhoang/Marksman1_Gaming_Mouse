#ifndef __BUTTON_SVC_H__
#define __BUTTON_SVC_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32_config.h"
#include "ht32.h"
#include "mouse_data.h"

/* Exported typedefs ---------------------------------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------------------------------------*/
void button_svc_init(void);
void button_svc(void);

#ifdef __cplusplus
}
#endif

#endif /* __BUTTON_SVC_H__ */

