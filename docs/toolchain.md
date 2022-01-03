# Toolchain

This document described the required steps you need to take to make the toolchain available. In this document I focus on a Windows development host, but it should be equally applicable to other OSs.

This document is based on the [SparkFun Advanced Development With the nRF5 SDK](https://learn.sparkfun.com/tutorials/nrf52840-advanced-development-with-the-nrf5-sdk) guide.

## nRF SDK

Extract the [nRF SDK for Thread and Zigbee](https://www.nordicsemi.com/Products/Development-software/nrf5-sdk-for-thread-and-zigbee): simply download it from the URL and extract it to a directory of your choice.

This project has been build with version 4.1.0 and assumes the SDK is installed in `D:\nRF5\nRF5_SDK_for_Thread_and_Zigbee_v4.1.0_32ce5f8` (but this can be adjusted later on, see the bottom of this file).

### Add board definition

> This step is no longer necessary, as this repository bundles its own board definition based on the SparkFun one.

You'll need to modify `components/boards/boards.h`, adding the following before `#elif defined(BOARD_CUSTOM)`:

```
#elif defined (BOARD_SPARKFUN_NRF52840_MINI)
  #include "sparkfun_nrf52840_mini.h"
```

Also download the [sparkfun_nrf52840_mini.h](https://raw.githubusercontent.com/sparkfun/nRF52840_Breakout_MDBT50Q/master/Firmware/nRF5_SDK/components/boards/sparkfun_nrf52840_mini.h) and place it in the same `components/boards` folder.

## ARM GNU Tools

Install the [ARM GNU Tools](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads): make sure to download and install version 7-2018-q2. This should be installed to `C:/Program Files (x86)/GNU Tools ARM Embedded/7 2018-q2-update`, which is the location configured in the nRF SDK.

## Adafruit nrfutil

Make sure you have Python 3 available, so you can install the Adafruit nrfutil to easily flash the nRF.

```
pip3 install --user adafruit-nrfutil
```

## Make

You need to have make available, you can use [gnuwin32](http://gnuwin32.sourceforge.net/packages/make.htm) for this. Just download and install, but make sure to add the installation directory to yous system `PATH`.

## Visual Studio Code

Make sure to install Visual Studio code. This project has been preconfigured for it, so it should run directly when everything is installed correctly.

You will need to install the "C/C++ Extension Pack" and the "F5 Anything" extensions within Visual Studio Code.

# Toolchain issues?

## I needed to use another directory for the installation

### nRF SDK

1. Adjust the `nrfSDK` path in `.vscode/c_cpp_properties.json`.
2. Adjust the `SDK_ROOT` path in `src/sparkfun_nrf52840_mini/blank/Makefile`, and make sure to remove the `src/sparkfun_nrf52840_mini/blank/_build` folder if it exists.

### GNU Tools

1. Adjust the `components/toolchain/gcc/Makefile.windows` file and update the `GNU_INSTALL_ROOT`.