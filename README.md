# MHI2ZIgbee for nRF52840

**THIS PROJECT IS CURRENTLY A WORK IN PROGRESS. IT IS NOT YET FUNCTIONAL!**

This project uses the [Sparkfun Pro nRF52840 Mini](https://www.sparkfun.com/products/15025) to connect with a Mitsubishi Heavy Industries (MHI) air conditioner (AC) using its SPI interface, and expose the controls using a standard Zigbee Clusters so it can be used with home automation over Zigbee.

This project has been inspired by and is based on the following open source MHI AC SPI implementations:
 - https://github.com/absalom-muc/MHI-AC-Ctrl
 - https://github.com/rjdekker/MHI2MQTT

For the code the Nordic nRF SDK examples have been used extensively.

## Used board

This project uses the [Sparkfun Pro nRF52840 Mini](https://www.sparkfun.com/products/15025) board as it provides an easy to use breakout board for the [Nordic nRF52840](https://www.nordicsemi.com/Products/nRF52840) SOC. It comes preprogrammed with a bootloader, which eases developement.

It is recommended to read the [Sparkfun Hookup Guide](https://learn.sparkfun.com/tutorials/sparkfun-pro-nrf52840-mini-hookup-guide) to learn more about the board and its capabilities.

## Pin layout

| Pin number | Function |
|---|---|
| 7 | Onboard LED, `BSP_BOARD_LED_0`, indicates whether the network has been joined |
| 9 | Zigbee reset, `BSP_BOARD_BUTTON_1`, connect with ground during reset to clear Zigbee storage |
| 10 | Helper LED, `BSP_BOARD_LED_1`, used to indicate AC on/off state |
| 17 | Log TX (see Logging) |

## Toolchain

In order to compile this library, you will need to have configured toolchain available. See [docs/toolchain.md](tree/main/docs/toolchain.md) for all details.

## Build & Run

1. Make sure to set the correct serial port in `.vscode/launch.json`. This is a `COM<number>`, you can see which ports are known by running `mode` in a command prompt.
1. Make sure to put the nRF in bootloader mode first by pressing the reset button twice. The led will blink continuously and it should also register itself as a USB storage device with Windows.
1. In Visual Studio code, press `Ctrl + F5` to build the project and upload it to the board.

## Reset Zigbee parameters

Connect pin 9 to ground and then use the reset button to reset the board. The Zigbee configuration will be cleared during boot.

> Do not forget to disconnect pin 9 again, as otherwise your configuration will be cleared once again!

## Logging

By default, this program uses pin 17 (TX) to log using the serial protocol, 115200 baud with 8 data bits, 1 stop bit, no parity and no flow control.
