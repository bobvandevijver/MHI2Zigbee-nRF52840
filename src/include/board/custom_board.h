#ifndef PROJECT_BOARD_H
#define PROJECT_BOARD_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "nrf_gpio.h"

#define LEDS_NUMBER 2

#define LED_1 NRF_GPIO_PIN_MAP(0, 7)
#define LED_2 NRF_GPIO_PIN_MAP(0, 10)
#define LED_START LED_1
#define LED_STOP LED_2

#define LEDS_ACTIVE_STATE 1

#define LEDS_LIST    \
    {                \
        LED_1, LED_2 \
    }

#define LEDS_INV_MASK LEDS_MASK

#define BSP_LED_0 7
#define BSP_LED_1 7

#define BUTTONS_NUMBER 2

#define BUTTON_1 13
#define BUTTON_2 9
#define BUTTON_PULL NRF_GPIO_PIN_PULLUP

#define BUTTONS_ACTIVE_STATE 0

#define BUTTONS_LIST       \
    {                      \
        BUTTON_1, BUTTON_2 \
    }

#define BSP_BUTTON_0 BUTTON_1
#define BSP_BUTTON_1 BUTTON_2

#define TX_PIN_NUMBER 17
#define HWFC false

#define APP_SPIS_SCK_PIN 29
#define APP_SPIS_MOSI_PIN 30
#define APP_SPIS_MISO_PIN 31

#ifdef __cplusplus
}
#endif

#endif // PROJECT_BOARD_H
