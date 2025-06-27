# Gurux.DLMS ANSI C Library

## Overview

See [Gurux](https://www.gurux.fi/ "Gurux") for an overview of our projects.

Join the Gurux community or follow [@Gurux](https://twitter.com/guruxorg "@Gurux") on Twitter for the latest updates.

Gurux.DLMS is a high-performance ANSI C library that allows you to read DLMS/COSEM-compatible electricity, gas, or water meters. We've designed it to be so user-friendly that you don't need to understand the protocol in detail.

For more information, visit [Gurux.DLMS](https://www.gurux.fi/Gurux.DLMS "Gurux.DLMS").

## Getting Started

We are continuously updating our documentation at the Gurux website.

Start by reading the [DLMS/COSEM FAQ](https://www.gurux.fi/DLMSCOSEMFAQ) and [DLMS Basics](https://www.gurux.fi/DLMSInNutshell).

Learn how to create your own [meter reading application](https://www.gurux.org/DLMSIntro) or build a custom DLMS/COSEM [meter, simulator, or proxy](https://www.gurux.fi/Gurux.DLMS.Server).

## Ideas and Discussions

If you have questions, ideas, or feedback, visit the Gurux [Forum](https://www.gurux.fi/forum). Please start a new topic for each new question. This is the best place for discussion, collaboration, and sharing insights.

## Contribute

If you'd like to contribute to the codebase, please sign the [Contributor Agreement](https://www.gurux.fi/files/ContributorAgreement.dotx).

## Reporting Issues and Feature Requests

DLMS is a complex protocol. Some theoretically good ideas might not work with all meters. Before reporting an issue, consider discussing it in the [Gurux Forum](https://www.gurux.fi/forum).

> **Note:** Inability to read a specific meter is not necessarily a software issue.

Report issues here: [Issue Tracker](https://www.gurux.fi/project/issues/guruxdlms.c)

## Licensing

Gurux products are [dual licensed](https://gurux.fi/OpenSource).

## Development Notes

You are not required to use Gurux media components such as `gurux.net.c` or `gurux.serial.c`. You may use any connection library. **Gurux.DLMS** handles the generation and parsing of DLMS data.

Source code is available on GitHub: [guruxdlms.c](https://www.github.com/gurux/guruxdlms.c)

## Implementing a Meter or Proxy

Start here: [Gurux.DLMS.Server](https://www.gurux.fi/Gurux.DLMS.Server)

See how COSEM objects are defined: [Gurux.DLMS.Objects](https://www.gurux.fi/Gurux.DLMS.Objects)

## Setup Instructions

Before starting, copy the necessary files from the `src` and `include` folders into your `dlms` folder.

To save memory, you can disable unused features via `CMakeLists.txt` like this:

```c
add_compile_definitions(GX_DLMS_MICROCONTROLLER DLMS_IGNORE_CLIENT DLMS_IGNORE_HIGH_SHA256 DLMS_IGNORE_HIGH_MD5 DLMS_USE_EPOCH_TIME GX_DLMS_SERIALIZER)
```

You can attach values to COSEM objects using ANSI C standard types:

```c
// Active power value. This will be updated.
unsigned long activePowerL1Value = 0;
// Attach value to COSEM attribute.
GX_UINT32_BYREF(activePowerL1.value) = &activePowerL1Value;
```

## Building the Firmware

This example uses the ESP-IDF Command Prompt. First, verify IDF installation:

```bash
idf.py --version
```

Set your target device (In this example it is ESP32-S3):

```bash
idf.py set-target esp32s3
```

Set the correct SSID and password:

```bash
idf.py menuconfig
# Component config → Wi-Fi Configuration → set SSID and password.
```
SSID and password are set only once.

Build the firmware:

```bash
idf.py build
```

Flash and monitor the firmware:

```bash
idf.py flash monitor
```

Meter IP address can be seen in the monotor output. It's something like this:
```bash
esp_netif_handlers: sta ip: 192.168.68.52, mask: 255.255.252.0, gw: 192.168.68.1
```

You can clear meter settings by running:
```bash
esptool.py erase_region 0x210000 0x10000
```
The offset (0x210000) and size (0x10000) depends from the used MCU.

This example uses a WRAPPER TCP/IP connection. For serial communication, the concept is the same—just manage the data send/receive over the serial port.

Keep your custom logic in `main.c`. This allows you to receive framework updates without conflict. Focus on your business logic—let the framework handle the rest.

## Example Meter Implementation

This example implements a DLMS meter with **WRAPPER** framing for TCP/IP connection using port 4059. 

The meter supports the following four association views:

- **None** — Client address: `0x10`
- **Low** — Client address: `0x11`, Password: `Gurux`
- **High** — Client address: `0x12`, Password: `Gurux`
- **GMac** — Client address: `0x01`, Password: `Gurux`
  - Authentication key: `D0D1D2D3D4D5D6D7D8D9DADBDCDDDEDF`
  - Block cipher key: `000102030405060708090A0B0C0D0E0F`
  - Logical name for invocation counter: `0.0.43.1.0.255`

