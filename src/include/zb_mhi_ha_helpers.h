/**
 * @file zb_mhi_ha_helpers.h
 * @brief Custom MHI device HA definitions
 */

#ifndef PROJECT_ZIGBEE_MHI_H
#define PROJECT_ZIGBEE_MHI_H 1

#include "zboss_api.h"

#define ZB_HA_MHI_IN_CLUSTER_NUM 5      /* MHI IN cluster number */
#define ZB_HA_MHI_OUT_CLUSTER_NUM 0     /* MHI output OUT cluster number */
#define ZB_HA_DEVICE_VER_HMI 0          /* MHI Output device version */
#define ZB_ZCL_MHI_REPORT_ATTR_COUNT 10 /* Number of attributes mandatory for reporting */

/**
 * @brief Declare cluster list for MHI device
 * @param cluster_list_name cluster list variable name
 * @param identify_attr_list attribute list for Identify cluster
 * @param basic_attr_list attribute list for Basic cluster
 * @param on_off_attr_list attribute list for On/Off cluster
 * @param fan_control_attr_list attribute list for Fan Control cluster
 * @param temp_measurement_attr_list attribute list for Temp Measurement cluster
 */
#define ZB_HA_DECLARE_MHI_CLUSTER_LIST(                                  \
    cluster_list_name,                                                   \
    identify_attr_list,                                                  \
    basic_attr_list,                                                     \
    on_off_attr_list,                                                    \
    fan_control_attr_list,                                               \
    temp_measurement_list)                                               \
    zb_zcl_cluster_desc_t cluster_list_name[] =                          \
        {                                                                \
            ZB_ZCL_CLUSTER_DESC(                                         \
                ZB_ZCL_CLUSTER_ID_IDENTIFY,                              \
                ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t),    \
                (identify_attr_list),                                    \
                ZB_ZCL_CLUSTER_SERVER_ROLE,                              \
                ZB_ZCL_MANUF_CODE_INVALID),                              \
            ZB_ZCL_CLUSTER_DESC(                                         \
                ZB_ZCL_CLUSTER_ID_BASIC,                                 \
                ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),       \
                (basic_attr_list),                                       \
                ZB_ZCL_CLUSTER_SERVER_ROLE,                              \
                ZB_ZCL_MANUF_CODE_INVALID),                              \
            ZB_ZCL_CLUSTER_DESC(                                         \
                ZB_ZCL_CLUSTER_ID_ON_OFF,                                \
                ZB_ZCL_ARRAY_SIZE(on_off_attr_list, zb_zcl_attr_t),      \
                (on_off_attr_list),                                      \
                ZB_ZCL_CLUSTER_SERVER_ROLE,                              \
                ZB_ZCL_MANUF_CODE_INVALID),                              \
            ZB_ZCL_CLUSTER_DESC(                                         \
                ZB_ZCL_CLUSTER_ID_FAN_CONTROL,                           \
                ZB_ZCL_ARRAY_SIZE(fan_control_attr_list, zb_zcl_attr_t), \
                (fan_control_attr_list),                                 \
                ZB_ZCL_CLUSTER_SERVER_ROLE,                              \
                ZB_ZCL_MANUF_CODE_INVALID),                              \
            ZB_ZCL_CLUSTER_DESC(                                         \
                ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,                      \
                ZB_ZCL_ARRAY_SIZE(temp_measurement_list, zb_zcl_attr_t), \
                (temp_measurement_list),                                 \
                ZB_ZCL_CLUSTER_SERVER_ROLE,                              \
                ZB_ZCL_MANUF_CODE_INVALID)}

/** @brief Declare simple descriptor for MHI device
 * @param ep_name endpoint variable name
 * @param ep_id endpoint ID
 * @param in_clust_num number of supported input clusters
 * @param out_clust_num number of supported output clusters
 * @note in_clust_num, out_clust_num should be defined by numeric constants, not variables or any
 * definitions, because these values are used to form simple descriptor type name
 */
#define ZB_ZCL_DECLARE_MHI_SIMPLE_DESC(ep_name, ep_id, in_clust_num, out_clust_num) \
    ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                            \
    ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num)                             \
    simple_desc_##ep_name =                                                         \
        {                                                                           \
            ep_id,                                                                  \
            ZB_AF_HA_PROFILE_ID,                                                    \
            ZB_HA_HEATING_COOLING_UNIT_DEVICE_ID,                                   \
            ZB_HA_DEVICE_VER_HMI,                                                   \
            0,                                                                      \
            in_clust_num,                                                           \
            out_clust_num,                                                          \
            {ZB_ZCL_CLUSTER_ID_BASIC,                                               \
             ZB_ZCL_CLUSTER_ID_IDENTIFY,                                            \
             ZB_ZCL_CLUSTER_ID_ON_OFF,                                              \
             ZB_ZCL_CLUSTER_ID_FAN_CONTROL,                                         \
             ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT}}

/**
 * @brief Declare endpoint for MHI device
 * @param ep_name endpoint variable name
 * @param ep_id endpoint ID
 * @param cluster_list endpoint cluster list
 */
#define ZB_HA_DECLARE_MHI_EP(ep_name, ep_id, cluster_list)      \
    ZB_ZCL_DECLARE_MHI_SIMPLE_DESC(                             \
        ep_name,                                                \
        ep_id,                                                  \
        ZB_HA_MHI_IN_CLUSTER_NUM,                               \
        ZB_HA_MHI_OUT_CLUSTER_NUM);                             \
    ZBOSS_DEVICE_DECLARE_REPORTING_CTX(                         \
        reporting_info##device_ctx_name,                        \
        ZB_ZCL_MHI_REPORT_ATTR_COUNT);                          \
    ZB_AF_DECLARE_ENDPOINT_DESC(                                \
        ep_name,                                                \
        ep_id,                                                  \
        ZB_AF_HA_PROFILE_ID,                                    \
        0,                                                      \
        NULL,                                                   \
        ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t), \
        cluster_list,                                           \
        (zb_af_simple_desc_1_1_t *)&simple_desc_##ep_name,      \
        ZB_ZCL_MHI_REPORT_ATTR_COUNT,                           \
        reporting_info##device_ctx_name,                        \
        0,                                                      \
        NULL)

/**
 * @brief Declare MHI device context
 * @param device_ctx device context variable name
 * @param ep_name endpoint variable name
 */
#define ZB_HA_DECLARE_MHI_CTX(device_ctx, ep_name) \
    ZBOSS_DECLARE_DEVICE_CTX_1_EP(device_ctx, ep_name)

#endif /* PROJECT_ZIGBEE_MHI_H */