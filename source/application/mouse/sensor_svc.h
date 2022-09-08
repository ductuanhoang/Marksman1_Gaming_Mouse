#ifndef __SENSOR_SVC_H__
#define __SENSOR_SVC_H__

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
void sensor_init(void);
void sensor_svc(void);

#ifdef __cplusplus
}
#endif

#endif /* __SENSOR_SVC_H__ */
