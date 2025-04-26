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