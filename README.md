# NFC_UICR_RK

*Library for manipulating the NFC settings in the UICR bytes on nRF52840 (Gen 3) devices.*

Particle Gen 3 devices (nRF52840 MCU) have the capability of supporting NFC tag mode.

| Device      | Platform        | NFC Enabled | NFC Disabled |
| :---------- | :-------------- | :---------: | :----------: |
| Argon       | `argon`         | &check;     |              |
| Boron       | `boron`         | &check;     |              |
| B-SoM       | `bsom`, `b5som` | &check;     |              |
| E404X       | `esomx`         |             | &check;      |
| Electron 2  | `electron2`     | &check;     |              |
| Monitor One | `tracker`       |             | &check;      |
| Tracker One | `tracker`       | &check;     |              |

The two devices that have NFC disabled from the factory (E-SoM and Monitor One) have it disabled because the pins are needed for additional GPIO. You also have the ability to do this on devices that have it enabled from the factory.

## Why is this process needed?

There's a diode that kicks in when the voltage differential between NFC1 and NFC2 is greater than 2.0V. This is designed to protect the MCU from induced voltage from the NFC coils, but obviously would have bad side effects if using as GPIO.

This protection diode can be disabled by setting a register in the the UICR bytes (nRF configuration bytes). These are extraordinarily persistent and will not be reset by flashing application firmware, Device OS, bootloader, or SoftDevice.

Normally you would change the UICR bytes using a SWD/JTAG programmer, but you can also do it from application firmware. One important caveat is that the MCU must be reset after updating the UICR bytes.

## Using SWD/JTAG

### SWD programming using hex files

Included in this Github are two files:

- `uicr_nfc_disable.hex`
- `uicr_nfc_enable.hex`

You can flash these using a SWD/JTAG programmer to update the settings. Reset the device after updating.

If you are using a CMSIS/DAP debugger you should be able to drag and drop it on the debugger volumne.

If you are using the Segger J/LINK with nrfjprog you might use a command like:

```
nfrjprog -f NRF52 --program uicr_nfc_disable.hex
```

### SWD programming manually

The NFC function is at offset 0x20C. The default value is enabled (0xffffffff) and to disable NFC, set bit 0 to 0, so 0xfffffffe. There's more information in the [Nordic UICR documentation](https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.nrf52832.ps.v1.1%2Fuicr.html). 

The offset is relative to the UICR base address 0x10001000, but the exact way to set it depends on which tool you are using. 

## Using this library

If you cannot use SWD to programmer during your device setup and initial programming, it is possible to do it from user firmware. This could be from the actual user firmware you will be using, or a setup or test firmware build.

One important caveat: The device must be reset after updating the UICR bytes. This is very fast and can be done from software so it doesn't require user intervention, but it does need to occur.

For example, you might include this in your firmware:

```cpp
#include "NFC_UICR_RK.h"

SYSTEM_MODE(SEMI_AUTOMATIC);

// System thread defaults to on in 6.2.0 and later and this line is not required
#ifndef SYSTEM_VERSION_v620
SYSTEM_THREAD(ENABLED);
#endif

SerialLogHandler logHandler(LOG_LEVEL_INFO);

void setup() {
    // Checks the NFC status in the UICR bytes. If enabled, disables it and resets the device.
    // You should use this with SYSTEM_THREAD(ENABLED) and SEMI_AUTOMATIC mode so the device
    // won't attempt to connect to the cloud before doing this!
    NFC_UICR_RK::disable();


    Particle.connect();
}

void loop() {

}
```
Of note:

- Be sure you use `SYSTEM_MODE(SEMI_AUTOMATIC)` so the device will not attempt to connect to the cloud before checking the UICR bytes.
- `SYSTEM_THREAD(ENABLED)` is always recommended in all firmware.
- Add a call to `NFC_UICR_RK::disable()` (or enable) at the beginning of `setup()`.
- Do not enable BLE before updating the UICR bytes.

If the UICR bytes are already in the correct enable/disable state, the function returns immediately.

If the UICR bytes need updating, they will be updated and the device reset, so the lines after it will not be executed until after reset.

One potential issue is that if the updating failed, the device would enter a rolling reboot and would never fully boot. This is unlikely, but cannot be ruled out as a remote possibility. Once the UICR bytes are set once, this should never be an issue.


