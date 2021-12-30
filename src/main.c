/* SDK includes */
#include "app_timer.h"
#include "bsp.h"
#include "boards.h"

/* Custom includes */
#include "include/zigbee.h"

/* SDK includes */
#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

/* Zigbee device context */
static mhi_device_ctx_t m_dev_ctx;

/* Declare the Zigbee cluster definitions */
ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST_EXT(
    basic_attr_list,
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
ZB_ZCL_DECLARE_IDENTIFY_ATTRIB_LIST(
    identify_attr_list,
    &m_dev_ctx.identify_attr.identify_time);
ZB_ZCL_DECLARE_ON_OFF_ATTRIB_LIST(
    on_off_attr_list,
    &m_dev_ctx.on_off_attr.on_off);
ZB_ZCL_DECLARE_FAN_CONTROL_ATTRIB_LIST(
    fan_control_attr_list,
    &m_dev_ctx.fan_control_attr.fan_mode,
    &m_dev_ctx.fan_control_attr.fan_mode_sequence);
ZB_ZCL_DECLARE_TEMP_MEASUREMENT_ATTRIB_LIST(
    temp_measurement_attr_list,
    &m_dev_ctx.temp_measurement_attr.measure_value,
    &m_dev_ctx.temp_measurement_attr.min_measure_value,
    &m_dev_ctx.temp_measurement_attr.max_measure_value,
    &m_dev_ctx.temp_measurement_attr.tolerance);

/* Declare the HA definitions */
ZB_HA_DECLARE_MHI_CLUSTER_LIST(
    mhi_clusters,
    identify_attr_list,
    basic_attr_list,
    on_off_attr_list,
    fan_control_attr_list,
    temp_measurement_attr_list);
ZB_HA_DECLARE_MHI_EP(mhi_ep, MHI_ENDPOINT, mhi_clusters);
ZB_HA_DECLARE_MHI_CTX(mhi_ctx, mhi_ep);

/**
 * @brief Function for the Timer initialization.
 * @details Initializes the timer module. This creates and starts application timers.
 */
static void timers_init(void)
{
    ret_code_t err_code;

    // Initialize timer module.
    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief Function for initializing the nrf log module.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

/**
 * @brief Function for turning ON/OFF the light bulb.
 * @param[in]   on   Boolean light bulb state.
 */
static void on_off_set_value(zb_bool_t on)
{
    NRF_LOG_INFO("Set ON/OFF value: %i", on);

    ZB_ZCL_SET_ATTRIBUTE(
        MHI_ENDPOINT,
        ZB_ZCL_CLUSTER_ID_ON_OFF,
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
        (zb_uint8_t *)&on,
        ZB_FALSE);

    if (on)
    {
        bsp_board_led_on(BSP_BOARD_LED_1);
    }
    else
    {
        bsp_board_led_off(BSP_BOARD_LED_1);
    }
}

/**
 * @brief Button handler function
 * @param event The nRF board event
 */
static void buttons_handler(bsp_event_t event)
{
    /* Inform default signal handler about user input at the device. */
    user_input_indicate();

    switch (event)
    {
    case BSP_EVENT_KEY_0:
        NRF_LOG_INFO("Button pressed");
        if (m_dev_ctx.on_off_attr.on_off)
        {
            on_off_set_value(ZB_FALSE);
        }
        else
        {
            on_off_set_value(ZB_TRUE);
        }
        break;

    default:
        NRF_LOG_INFO("Unhandled BSP Event received: %d", event);
        return;
    }
}

/**
 * @brief Function for initializing LEDs and buttons.
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

/**
 * @brief Zigbee stack event handler.
 * @param[in]   bufid   Reference to the Zigbee stack buffer used to pass signal.
 */
void zboss_signal_handler(zb_bufid_t bufid)
{
    zb_zdo_app_signal_hdr_t *p_sg_p = NULL;
    zb_zdo_app_signal_type_t sig = zb_get_app_signal(bufid, &p_sg_p);

    /* Update network status LED */
    zigbee_led_status_update(bufid, ZIGBEE_NETWORK_STATE_LED);

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

/**
 * @brief Initialize the cluster data
 */
static void mhi_clusters_attr_init(void)
{
    /* Basic cluster attributes data */
    m_dev_ctx.basic_attr.zcl_version = ZB_ZCL_VERSION;
    m_dev_ctx.basic_attr.app_version = MHI_INIT_BASIC_APP_VERSION;
    m_dev_ctx.basic_attr.stack_version = MHI_INIT_BASIC_STACK_VERSION;
    m_dev_ctx.basic_attr.hw_version = MHI_INIT_BASIC_HW_VERSION;
    m_dev_ctx.basic_attr.power_source = MHI_INIT_BASIC_POWER_SOURCE;
    m_dev_ctx.basic_attr.ph_env = MHI_INIT_BASIC_PH_ENV;

    /* Use ZB_ZCL_SET_STRING_VAL to set strings, because the first byte should
     * contain string length without trailing zero.
     *
     * For example "test" string wil be encoded as:
     *   [(0x4), 't', 'e', 's', 't']
     */
    ZB_ZCL_SET_STRING_VAL(
        m_dev_ctx.basic_attr.mf_name,
        MHI_INIT_BASIC_MANUF_NAME,
        ZB_ZCL_STRING_CONST_SIZE(MHI_INIT_BASIC_MANUF_NAME));

    ZB_ZCL_SET_STRING_VAL(
        m_dev_ctx.basic_attr.model_id,
        MHI_INIT_BASIC_MODEL_ID,
        ZB_ZCL_STRING_CONST_SIZE(MHI_INIT_BASIC_MODEL_ID));

    ZB_ZCL_SET_STRING_VAL(
        m_dev_ctx.basic_attr.date_code,
        MHI_INIT_BASIC_DATE_CODE,
        ZB_ZCL_STRING_CONST_SIZE(MHI_INIT_BASIC_DATE_CODE));

    /* Identify cluster attributes data */
    m_dev_ctx.identify_attr.identify_time = ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE;

    /* On/Off cluster attributes data */
    m_dev_ctx.on_off_attr.on_off = (zb_bool_t)ZB_ZCL_ON_OFF_IS_OFF;

    ZB_ZCL_SET_ATTRIBUTE(
        MHI_ENDPOINT,
        ZB_ZCL_CLUSTER_ID_ON_OFF,
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
        (zb_uint8_t *)&m_dev_ctx.on_off_attr.on_off,
        ZB_FALSE);
}

/**
 * @brief ZCL callback function
 * @param bufid Buffer handler
 */
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
            NRF_LOG_INFO("Unhandled cluster (%d) attribute (%d)", cluster_id, attr_id);
        }
        break;

    default:
        p_device_cb_param->status = RET_ERROR;
        break;
    }

    NRF_LOG_INFO("zcl_device_cb status: %hd", p_device_cb_param->status);
}

/**
 * @brief Main application function
 */
int main(void)
{
    zb_ret_t zb_err_code;
    zb_ieee_addr_t ieee_addr;

    /* Initialize timers, loging system and GPIOs. */
    timers_init();
    log_init();
    leds_buttons_init();

    /* Log booting message */
    NRF_LOG_INFO("Booting...");
    NRF_LOG_FLUSH();

    bsp_board_leds_on();
    nrf_delay_ms(500);
    bsp_board_leds_off();

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
    zb_set_network_ed_role(ZB_TRANSCEIVER_ALL_CHANNELS_MASK);
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
    NRF_LOG_FLUSH();

    while (1)
    {
        zboss_main_loop_iteration();
        UNUSED_RETURN_VALUE(NRF_LOG_PROCESS());
    }
}
