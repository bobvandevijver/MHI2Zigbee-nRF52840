# MHI2ZIgbee for nRF52840

**THIS PROJECT IS CURRENTLY A WORK IN PROGRESS. IT IS NOT YET FUNCTIONAL!**

This project uses the [Sparkfun Pro nRF52840 Mini](https://www.sparkfun.com/products/15025) to connect with a Mitsubishi Heavy Industries (MHI) air conditioner (AC) using its SPI interface, and expose the controls using a standard Zigbee Clusters so it can be used with home automation over Zigbee.

This project has been inspired by and is based on the following open source MHI AC SPI implementations:
 - https://github.com/absalom-muc/MHI-AC-Ctrl
 - https://github.com/rjdekker/MHI2MQTT

For the code the Nordic nRF SDK examples have been used extensively.

## Used board

This project uses the [Sparkfun Pro nRF52840 Mini](https://www.sparkfun.com/products/15025) board as it provides an easy to use breakout board for the [Nordic nRF52840(https://www.nordicsemi.com/Products/nRF52840)] SOC. It comes preprogrammed with a bootloader, which eases developement.

It is recommended to read the [Sparkfun Hookup Guide](https://learn.sparkfun.com/tutorials/sparkfun-pro-nrf52840-mini-hookup-guide) to learn more about the board and its capabilities.

## Toolchain

In order to compile this library, you will need to have configured toolchain available. See [docs/toolchain.md](tree/main/docs/toolchain.md) for all details.

## Build & Run

In Visual Studio code, simply press `Ctrl + F5` to build the project and upload it to the board. Make sure to set the correct serial port in `.vscode/launch.json` though.
