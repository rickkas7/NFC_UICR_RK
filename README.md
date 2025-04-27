# NFC_UICR_RK

*Library for manipulating the NFC settings in the UICR bytes on nRF52840 (Gen 3) devices.*

Particle Gen 3 devices (nRF52840 MCU) have the capability of supporting NFC tag mode.

| Device      | Platform        | NFC Enabled | NFC Disabled |
| :---------- | :-------------- | :---------: | :----------: |
| Argon       | `argon`         | x           |              |
| Boron       | `boron`         | x           |              |
| B-SoM       | `bsom`, `b5som` | x           |              |
| E404X       | `esomx`         |             | x            |
| Electron 2  | `electron2`     | x           |              |
| Monitor One | `tracker`       |             | x            |
| Tracker One | `tracker`       | x           |              |

The two devices that have NFC disabled from the factory (E-SoM and Monitor One) have it disabled because the pins are needed for additional GPIO. 

You also have the ability to do this on devices that have it enabled from the factory. Reusing NFC pins as GPIO really only makes sense on the B-SoM because the NFC pins are included on M.2 connector. On the Argon, Boron, and Electron 2 the NFC pins are only available on a U.FL connector. 

- Github: https://github.com/rickkas7/NFC_UICR_RK
- License: MIT
- Full browsable API documentation available [here](https://rickkas7.github.io/NFC_UICR_RK/index.html) as well as in the docs directory 

## Why is this process needed?

There's a diode that kicks in when the voltage differential between NFC1 and NFC2 is greater than 2.0V. This is designed to protect the MCU from induced voltage from the NFC coils, but obviously would have bad side effects if using as GPIO.

This protection diode can be disabled by setting a register in the the UICR bytes (nRF52 configuration bytes). These are  persistent and will not be reset by flashing application firmware, Device OS, bootloader, or SoftDevice.

Normally you would change the UICR bytes using a SWD/JTAG programmer, but you can also do it from application firmware. One important caveat is that the MCU must be reset after updating the UICR bytes.

You can only set the UICR byte bits from 1 to 0 in application firmware or only by flashing the UICR values! In order to 
set a 0 value (NFC disabled) back to 1 (NFC enabled), you need to chip erase the device which reset the UICR back to 1 values. It will also erase the bootloader, soft devies, Device OS, and application firmware.

Device Restore USB does not affect the UICR bytes.

## Using SWD/JTAG

### SWD programming using hex files

Included in this Github are two files:

- `uicr_nfc_disable.hex`
- `uicr.hex`

You can flash these using a SWD/JTAG programmer to update the settings. Reset the device after updating.

If you are using a CMSIS/DAP debugger you should be able to drag and drop `uicr_nfc_disable.hex` on the debugger DAPLINK volume.

If you are using the Segger J/LINK with nrfjprog you might use a command like:

```
nfrjprog -f NRF52 --program uicr_nfc_disable.hex
```

### Restoring UICR bytes

To restore the UICR bytes:

```
nfrjprog -f NRF52 --program uicr.hex --chiperase
nfrjprog -f NRF52 --program device-restore-image.hex
```

You cannot restore UICR bytes with a CMSIS/DAP debugger drag-and-drop but you may be able to with OpenOCD.

### SWD programming manually

The NFC function is at offset 0x20C. The default value is enabled (0xffffffff) and to disable NFC, set bit 0 to 0, so 0xfffffffe. There's more information in the [Nordic UICR documentation](https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.nrf52832.ps.v1.1%2Fuicr.html). 

The offset is relative to the UICR base address 0x10001000, but the exact way to set it depends on which tool you are using. 

You cannot set single UICR bits back to 1; you must chip erase and flash the entire device image to restore NFC mode.

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
    // Checks the NFC status in the UICR bytes and disables it if enabled.
    // You should use this with SYSTEM_THREAD(ENABLED) and SEMI_AUTOMATIC mode so the device
    // won't attempt to connect to the cloud before doing this!
    NFC_UICR_RK::disableNFC();

    // Can be any time after disableNFC() returns
    Particle.connect();
}

void loop() {

}
```

Of note:

- Be sure you use `SYSTEM_MODE(SEMI_AUTOMATIC)` so the device will not attempt to connect to the cloud before checking the UICR bytes.
- `SYSTEM_THREAD(ENABLED)` is always recommended in all firmware.
- Add a call to `NFC_UICR_RK::disableNFC()` at the beginning of `setup()`.
- Do not enable BLE before updating the UICR bytes.
- It's safe to call `Particle.connect()` and time after checking the UICR bytes.

If the UICR bytes indicate that NFC is already disabled, the function returns immediately.

If the UICR bytes need updating, they will be updated and the device reset, so the lines after it will not be executed until after reset.

One potential issue is that if the updating failed, the device would enter a rolling reboot and would never fully boot. This is unlikely, but cannot be ruled out as a remote possibility. Once the UICR bytes are set once, this should never be an issue.

Note that you cannot turn NFC back on without chip erasing the device using a SWD/JTAG programmer, so make sure you don't turn it off if you may need to use it again and you don't have the appropriate hardware!


