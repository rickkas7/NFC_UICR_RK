#include "NFC_UICR_RK.h"

SYSTEM_MODE(SEMI_AUTOMATIC);

// System thread defaults to on in 6.2.0 and later and this line is not required
#ifndef SYSTEM_VERSION_v620
SYSTEM_THREAD(ENABLED);
#endif

SerialLogHandler logHandler(LOG_LEVEL_INFO);

unsigned long lastCheckMs = 0;
std::chrono::milliseconds checkPeriodMs = 10s;
std::chrono::milliseconds ledPeriodMs = 2s;

void setup() {
    Particle.connect();
}

void loop() {
    if (millis() - lastCheckMs >= checkPeriodMs.count()) {
        lastCheckMs = millis();

        bool nfcEnabled = NFC_UICR_RK::checkEnableNFC();
        
        Log.info("nfcEnabled=%d", (int)nfcEnabled);

        RGB.control(true);
        RGB.brightness(255);

        if (nfcEnabled) {
            // Green = NFC enabled
            RGB.color(0, 255, 0);
        }
        else {
            // Blue = NFC disabled
            RGB.color(0, 0, 255);
        }

        delay(ledPeriodMs);

        RGB.control(false);
    }
}
