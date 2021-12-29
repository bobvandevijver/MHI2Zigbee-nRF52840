#include "zboss_api.h"
#include "zboss_api_addons.h"
#include "zb_mem_config_custom.h"
#include "zb_error_handler.h"
#include "zigbee_helpers.h"

#include "app_timer.h"
#include "bsp.h"
#include "boards.h"

#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define MHI_ZIGBEE_CHANNEL 11
#define MHI_ENDPOINT 1                                                  /**< Device endpoint, used to receive controlling commands. */
#define MHI_INIT_BASIC_APP_VERSION 01                                   /**< Version of the application software (1 byte). */
#define MHI_INIT_BASIC_STACK_VERSION 10                                 /**< Version of the implementation of the Zigbee stack (1 byte). */
#define MHI_INIT_BASIC_HW_VERSION 11                                    /**< Version of the hardware of the device (1 byte). */
#define MHI_INIT_BASIC_MANUF_NAME "bobvandevijver"                      /**< Manufacturer name (32 bytes). */
#define MHI_INIT_BASIC_MODEL_ID "mhi-ac-control"                        /**< Model number assigned by manufacturer (32-bytes long string). */
#define MHI_INIT_BASIC_DATE_CODE "20211229"                             /**< First 8 bytes specify the date of manufacturer of the device in ISO 8601 format (YYYYMMDD). The rest (8 bytes) are manufacturer specific. */
#define MHI_INIT_BASIC_POWER_SOURCE ZB_ZCL_BASIC_POWER_SOURCE_DC_SOURCE /**< Type of power sources available for the device. For possible values see section 3.2.2.2.8 of ZCL specification. */
#define MHI_INIT_BASIC_PH_ENV ZB_ZCL_BASIC_ENV_UNSPECIFIED              /**< Describes the type of physical environment. For possible values see section 3.2.2.2.10 of ZCL specification. */

#define IEEE_CHANNEL_MASK (1l << MHI_ZIGBEE_CHANNEL) /**< Scan only one, predefined channel to find the coordinator. */
#define ERASE_PERSISTENT_CONFIG ZB_FALSE             /**< Do not erase NVRAM to save the network parameters after device reboot or power-off. NOTE: If this option is set to ZB_TRUE then do full device erase for all network devices before running other samples. */
// #define ZIGBEE_NETWORK_STATE_LED BSP_BOARD_LED_0             /**< LED indicating that light switch successfully joind Zigbee network. */

#if !defined ZB_ED_ROLE
#error Define ZB_ED_ROLE to compile light switch (End Device) source code.
#endif

/* Main application customizable context. Stores all settings and static values. */
typedef struct
{
    zb_zcl_basic_attrs_ext_t basic_attr;
    zb_zcl_identify_attrs_t identify_attr;
    zb_zcl_on_off_attrs_t on_off_attr;
    zb_zcl_scenes_attrs_t scenes_attr;
    zb_zcl_groups_attrs_t groups_attr;
} mhi_device_ctx_t;

static mhi_device_ctx_t m_dev_ctx;

/* Declare the Zigbee cluster definitions */
ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST_EXT(basic_attr_list,
                                     &m_dev_ctx.basic_attr.zcl_version,
                                     &m_dev_ctx.basic_attr.app_version,
                                     &m_dev_ctx.basic_attr.stack_version,
                                     &m_dev_ctx.basic_attr.hw_version,
                                     m_dev_ctx.basic_attr.mf_name,
                                     m_dev_ctx.basic_attr.model_id,
                                     m_dev_ctx.basic_attr.date_code,
                                     &m_dev_ctx.basic_attr.power_source,
                                     m_dev_ctx.basic_attr.location_id,
                                     &m_dev_ctx.basic_attr.ph_env,
                                     m_dev_ctx.basic_attr.sw_ver);
ZB_ZCL_DECLARE_IDENTIFY_ATTRIB_LIST(identify_attr_list, &m_dev_ctx.identify_attr.identify_time);
ZB_ZCL_DECLARE_ON_OFF_ATTRIB_LIST(on_off_attr_list, &m_dev_ctx.on_off_attr.on_off);
ZB_ZCL_DECLARE_GROUPS_ATTRIB_LIST(groups_attr_list, &m_dev_ctx.groups_attr.name_support);
ZB_ZCL_DECLARE_SCENES_ATTRIB_LIST(scenes_attr_list,
                                  &m_dev_ctx.scenes_attr.scene_count,
                                  &m_dev_ctx.scenes_attr.current_scene,
                                  &m_dev_ctx.scenes_attr.current_group,
                                  &m_dev_ctx.scenes_attr.scene_valid,
                                  &m_dev_ctx.scenes_attr.name_support);

/* Declare the HA definitions */
ZB_HA_DECLARE_ON_OFF_OUTPUT_CLUSTER_LIST(mhi_clusters, on_off_attr_list, basic_attr_list, identify_attr_list, groups_attr_list, scenes_attr_list);
ZB_HA_DECLARE_ON_OFF_OUTPUT_EP(mhi_ep, MHI_ENDPOINT, mhi_clusters);
ZB_HA_DECLARE_ON_OFF_OUTPUT_CTX(mhi_ctx, mhi_ep);

/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
static void timers_init(void)
{
    ret_code_t err_code;

    // Initialize timer module.
    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing the nrf log module.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

static void buttons_handler(bsp_event_t event)
{
    /* Inform default signal handler about user input at the device. */
    user_input_indicate();

    switch (event)
    {
    case BSP_EVENT_KEY_0:
        NRF_LOG_INFO("Button pressed");
        break;

    default:
        NRF_LOG_INFO("Unhandled BSP Event received: %d", event);
        return;
    }
}

/**@brief Function for initializing LEDs and buttons.
 */
static zb_void_t leds_buttons_init(void)
{
    ret_code_t error_code;

    /* Initialize LEDs and buttons - use BSP to control them. */
    error_code = bsp_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS, buttons_handler);
    APP_ERROR_CHECK(error_code);
    /* By default the bsp_init attaches BSP_KEY_EVENTS_{0-4} to the PUSH events of the corresponding buttons. */

    bsp_board_leds_off();
}

/**@brief Zigbee stack event handler.
 *
 * @param[in]   bufid   Reference to the Zigbee stack buffer used to pass signal.
 */
void zboss_signal_handler(zb_bufid_t bufid)
{
    zb_zdo_app_signal_hdr_t *p_sg_p = NULL;
    zb_zdo_app_signal_type_t sig = zb_get_app_signal(bufid, &p_sg_p);

    /* Update network status LED */
    // zigbee_led_status_update(bufid, ZIGBEE_NETWORK_STATE_LED);

    switch (sig)
    {
    case ZB_BDB_SIGNAL_DEVICE_REBOOT:
        /* fall-through */
    case ZB_BDB_SIGNAL_STEERING:
        /* Call default signal handler. */
        ZB_ERROR_CHECK(zigbee_default_signal_handler(bufid));
        break;

    default:
        /* Call default signal handler. */
        ZB_ERROR_CHECK(zigbee_default_signal_handler(bufid));
        break;
    }

    if (bufid)
    {
        zb_buf_free(bufid);
    }
}

static void mhi_clusters_attr_init(void)
{
    /* Basic cluster attributes data */
    m_dev_ctx.basic_attr.zcl_version = ZB_ZCL_VERSION;
    m_dev_ctx.basic_attr.app_version = MHI_INIT_BASIC_APP_VERSION;
    m_dev_ctx.basic_attr.stack_version = MHI_INIT_BASIC_STACK_VERSION;
    m_dev_ctx.basic_attr.hw_version = MHI_INIT_BASIC_HW_VERSION;

    /* Use ZB_ZCL_SET_STRING_VAL to set strings, because the first byte should
     * contain string length without trailing zero.
     *
     * For example "test" string wil be encoded as:
     *   [(0x4), 't', 'e', 's', 't']
     */
    ZB_ZCL_SET_STRING_VAL(m_dev_ctx.basic_attr.mf_name,
                          MHI_INIT_BASIC_MANUF_NAME,
                          ZB_ZCL_STRING_CONST_SIZE(MHI_INIT_BASIC_MANUF_NAME));

    ZB_ZCL_SET_STRING_VAL(m_dev_ctx.basic_attr.model_id,
                          MHI_INIT_BASIC_MODEL_ID,
                          ZB_ZCL_STRING_CONST_SIZE(MHI_INIT_BASIC_MODEL_ID));

    ZB_ZCL_SET_STRING_VAL(m_dev_ctx.basic_attr.date_code,
                          MHI_INIT_BASIC_DATE_CODE,
                          ZB_ZCL_STRING_CONST_SIZE(MHI_INIT_BASIC_DATE_CODE));

    m_dev_ctx.basic_attr.power_source = MHI_INIT_BASIC_POWER_SOURCE;

    m_dev_ctx.basic_attr.ph_env = MHI_INIT_BASIC_PH_ENV;

    /* Identify cluster attributes data */
    m_dev_ctx.identify_attr.identify_time = ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE;

    /* On/Off cluster attributes data */
    m_dev_ctx.on_off_attr.on_off = (zb_bool_t)ZB_ZCL_ON_OFF_IS_OFF;

    ZB_ZCL_SET_ATTRIBUTE(MHI_ENDPOINT,
                         ZB_ZCL_CLUSTER_ID_ON_OFF,
                         ZB_ZCL_CLUSTER_SERVER_ROLE,
                         ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
                         (zb_uint8_t *)&m_dev_ctx.on_off_attr.on_off,
                         ZB_FALSE);
}

/**@brief Function for turning ON/OFF the light bulb.
 *
 * @param[in]   on   Boolean light bulb state.
 */
static void on_off_set_value(zb_bool_t on)
{
    NRF_LOG_INFO("Set ON/OFF value: %i", on);

    ZB_ZCL_SET_ATTRIBUTE(MHI_ENDPOINT,
                         ZB_ZCL_CLUSTER_ID_ON_OFF,
                         ZB_ZCL_CLUSTER_SERVER_ROLE,
                         ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
                         (zb_uint8_t *)&on,
                         ZB_FALSE);

    if (on)
    {
        bsp_board_led_on(0);
    }
    else
    {
        bsp_board_led_off(0);
    }
}

static zb_void_t zcl_device_cb(zb_bufid_t bufid)
{
    zb_uint8_t cluster_id;
    zb_uint8_t attr_id;
    zb_zcl_device_callback_param_t *p_device_cb_param = ZB_BUF_GET_PARAM(bufid, zb_zcl_device_callback_param_t);

    NRF_LOG_INFO("zcl_device_cb id %hd", p_device_cb_param->device_cb_id);

    /* Set default response value. */
    p_device_cb_param->status = RET_OK;

    switch (p_device_cb_param->device_cb_id)
    {
    case ZB_ZCL_SET_ATTR_VALUE_CB_ID:
        cluster_id = p_device_cb_param->cb_param.set_attr_value_param.cluster_id;
        attr_id = p_device_cb_param->cb_param.set_attr_value_param.attr_id;

        if (cluster_id == ZB_ZCL_CLUSTER_ID_ON_OFF)
        {
            uint8_t value = p_device_cb_param->cb_param.set_attr_value_param.values.data8;

            NRF_LOG_INFO("on/off attribute setting to %hd", value);
            if (attr_id == ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID)
            {
                on_off_set_value((zb_bool_t)value);
            }
        }
        else
        {
            /* Other clusters can be processed here */
            NRF_LOG_INFO("Unhandled cluster attribute id: %d", cluster_id);
        }
        break;

    default:
        p_device_cb_param->status = RET_ERROR;
        break;
    }

    NRF_LOG_INFO("zcl_device_cb status: %hd", p_device_cb_param->status);
}

int main(void)
{
    zb_ret_t zb_err_code;
    zb_ieee_addr_t ieee_addr;

    /* Initialize timers, loging system and GPIOs. */
    timers_init();
    log_init();
    leds_buttons_init();

    /* Set Zigbee stack logging level and traffic dump subsystem. */
    ZB_SET_TRACE_LEVEL(ZIGBEE_TRACE_LEVEL);
    ZB_SET_TRACE_MASK(ZIGBEE_TRACE_MASK);
    ZB_SET_TRAF_DUMP_OFF();

    /* Initialize Zigbee stack. */
    ZB_INIT("MHI_AC");

    /* Set device address to the value read from FICR registers. */
    zb_osif_get_ieee_eui64(ieee_addr);
    zb_set_long_address(ieee_addr);

    /* Set static long IEEE address. */
    zb_set_network_ed_role(IEEE_CHANNEL_MASK);
    zigbee_erase_persistent_storage(ERASE_PERSISTENT_CONFIG);

    zb_set_ed_timeout(ED_AGING_TIMEOUT_64MIN);
    zb_set_keepalive_timeout(ZB_MILLISECONDS_TO_BEACON_INTERVAL(3000));

    /* Initialize application context structure. */
    UNUSED_RETURN_VALUE(ZB_MEMSET(&m_dev_ctx, 0, sizeof(mhi_device_ctx_t)));

    /* Register callback for handling ZCL commands. */
    ZB_ZCL_REGISTER_DEVICE_CB(zcl_device_cb);

    /* Register dimmer switch device context (endpoints). */
    ZB_AF_REGISTER_DEVICE_CTX(&mhi_ctx);

    mhi_clusters_attr_init();

    /** Start Zigbee Stack. */
    zb_err_code = zboss_start_no_autostart();
    ZB_ERROR_CHECK(zb_err_code);

    while (1)
    {
        zboss_main_loop_iteration();
        UNUSED_RETURN_VALUE(NRF_LOG_PROCESS());
    }
}
