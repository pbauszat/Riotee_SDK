[![Build](https://github.com/NessieCircuits/Riotee_Runtime/actions/workflows/build.yml/badge.svg)](https://github.com/NessieCircuits/Riotee_Runtime/actions/workflows/build.yml)

# Riotee Software Development Kit

The code in this repository allows building applications that run on battery-free Riotee devices.
For a detailed description refer to the [documentation](https://www.riotee.nessie-circuits.de/docs/software/riotee-runtime).

For a quickstart with Riotee you can use our Arduino package which internally makes use of the SDK. For more involved projects, continue reading here.

## Installation

Download the [latest zip release](https://github.com/NessieCircuits/Riotee_Runtime/releases/latest) and unpack it to a path on your machine.

Alternatively, clone this repository to get the latest development version of the SDK:

```bash
git clone --recursive git@github.com:NessieCircuits/Riotee_Runtime.git
```

### Linux

 - Install `make` from your distribution's repository (`apt install build-essential` on Ubuntu).
 - Install the `GNU Arm Embedded Toolchain` from your distribution's repository (`apt install gcc-arm-none-eabi` on Ubuntu) or from [the official website](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads).
 - For uploading firmware to your Riotee device install [pyOCD](https://pyocd.io/) with

```bash
pip install pyocd
```

### Windows
 - Install `make` via Chocolatey or Cygwin following the instructions [here](https://earthly.dev/blog/makefiles-on-windows/).
 - Add `C:/cygwin64/bin` to your `Path` variable ([Instructions](https://www.architectryan.com/2018/03/17/add-to-the-path-on-windows-10/)).
 - Install the `GNU Arm Embedded Toolchain` from the [official website](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads).
 - For uploading firmware to your Riotee device install [pyOCD](https://pyocd.io/) with

```bash
pip install pyocd
```
## Usage

Take a look at the [examples](./examples) for how you can use this SDK. We also provide a [template project](https://github.com/NessieCircuits/Riotee_AppTemplate) to get you started with your first Riotee project using the SDK.

In a nutshell: Your application must provide a Makefile that includes the [SDK's Makefile](./Makefile) and defines the following variables:

 - `RIOTEE_SDK_ROOT`: Path to the Riotee SDK
 - `SRC_FILES`: Your C/C++ source files
 - `INC_FOLDERS`: Paths to your header files
 - `OUTPUT_DIR`: Path where project is built
 - `GNU_INSTALL_ROOT`: Optional. Path to your gcc toolchain if not on your *Path*.

 Your code defines a `main()` function. This function is executed while the device has energy and suspended when energy becomes critically low. If the power supply is interrupted, the stack and all static and global variables are stored in non-volatile memory and restored as soon as the supply comes back.

There are a number of callbacks that your application can implement:
 - `startup_callback()`: Called right after every reset. Perform early stage initizialization required for low-power operation here
 - `reset_callback()`: Called later after every reset. Initialize peripherals here.
 - `turnoff_callback()`: Called right before the application gets suspended. Abort any energy-intensive operation immediately.


## Features

 - Capacitor voltage monitoring
 - Driver for non-volatile RAM
 - Automatic checkpointing of user application
 - C++ support
 - Basic timing support
 - printf support
 - BLE advertising
 - *Stella* wireless protocol for bidirectional communication with a basestation
 - Drivers:
   - ADC
   - UART
   - I2C
   - SPI controller
   - MAX20361 boost convert
   - AM1805 RTC
   - VM1010 microphone
   - SHTC3 T&H sensor

## Code structure

 - `/core`: Riotee runtime and peripheral drivers
 - `/drivers`: Drivers for external devices
 - `/examples`: Examples using the SDK
 - `/external`: External dependencies
