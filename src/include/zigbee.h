/**
 * @file zigbee.h
 * @brief Contains the used Zigbee configuration for the project
 */

#ifndef PROJECT_ZIGBEE_H
#define PROJECT_ZIGBEE_H 1

/* SDK includes */
#include "zboss_api.h"
#include "zboss_api_addons.h"
#include "zb_mem_config_custom.h"
#include "zb_error_handler.h"
#include "zigbee_helpers.h"

/* Custom includes */
#include "zb_mhi_ha_helpers.h"

#define MHI_ENDPOINT 1                                                  /**< Device endpoint, used to receive controlling commands. */
#define MHI_INIT_BASIC_APP_VERSION 01                                   /**< Version of the application software (1 byte). */
#define MHI_INIT_BASIC_STACK_VERSION 10                                 /**< Version of the implementation of the Zigbee stack (1 byte). */
#define MHI_INIT_BASIC_HW_VERSION 11                                    /**< Version of the hardware of the device (1 byte). */
#define MHI_INIT_BASIC_MANUF_NAME "bobvandevijver"                      /**< Manufacturer name (32 bytes). */
#define MHI_INIT_BASIC_MODEL_ID "mhi-ac-control"                        /**< Model number assigned by manufacturer (32-bytes long string). */
#define MHI_INIT_BASIC_DATE_CODE "20211229"                             /**< First 8 bytes specify the date of manufacturer of the device in ISO 8601 format (YYYYMMDD). The rest (8 bytes) are manufacturer specific. */
#define MHI_INIT_BASIC_POWER_SOURCE ZB_ZCL_BASIC_POWER_SOURCE_DC_SOURCE /**< Type of power sources available for the device. For possible values see section 3.2.2.2.8 of ZCL specification. */
#define MHI_INIT_BASIC_PH_ENV ZB_ZCL_BASIC_ENV_UNSPECIFIED              /**< Describes the type of physical environment. For possible values see section 3.2.2.2.10 of ZCL specification. */

#define ERASE_PERSISTENT_CONFIG ZB_FALSE         /**< Do not erase NVRAM to save the network parameters after device reboot or power-off. NOTE: If this option is set to ZB_TRUE then do full device erase for all network devices before running other samples. */
#define ZIGBEE_NETWORK_STATE_LED BSP_BOARD_LED_0 /**< LED indicating that light switch successfully joind Zigbee network. */

#if !defined ZB_ED_ROLE
#error Define ZB_ED_ROLE to compile light switch (End Device) source code.
#endif

/* Not defined by zboss */
typedef struct zb_zcl_fan_control_attrs_s
{
    zb_uint8_t fan_mode;
    zb_uint8_t fan_mode_sequence;
} zb_zcl_fan_control_attrs_t;

/* Main application customizable context. Stores all settings and static values. */
typedef struct
{
    zb_zcl_basic_attrs_ext_t basic_attr;
    zb_zcl_identify_attrs_t identify_attr;
    zb_zcl_on_off_attrs_t on_off_attr;
    zb_zcl_fan_control_attrs_t fan_control_attr;
    zb_zcl_temp_measurement_attrs_t temp_measurement_attr;
} mhi_device_ctx_t;

#endif /* PROJECT_ZIGBEE_H */