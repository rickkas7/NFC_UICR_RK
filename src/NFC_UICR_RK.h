#ifndef __NFC_UICR_RK_H
#define __NFC_UICR_RK_H

// Github: https://github.com/rickkas7/NFC_UICR_RK
// License: MIT

#include "Particle.h"

/**
 * @brief Class for updating the nRF52840 UICR bytes to disable NFC
 * 
 * You will typically call NFC_UICR_RK::disableNFC() to disable NFC on devices that have it enabled but you
 * want to use the pins as GPIO. The E404X and Monitor One already have it disabled from the factory.
 * 
 * If the state of NFC is enabled it will be updated. There are caveats to this:
 * - You should make the call as close as possible to the beginning of setup()
 * - Use SYSTEM_MODE(SEMI_AUTOMATIC) 
 * - Particle.connect() any time after calling one of the functions, or at the end of setup()
 * - Don't enable BLE before calling one of these functions
 * 
 * The reason is that if the UICR bits need to be updated, SoftDevice (BLE) needs to be disabled. It
 * will also System.reset() after updating the bits. Because of this, there is a possibility that the code
 * after enable() or disable() will never be executed.
 * 
 * There is no enableNFC()! This is because in order to reenable NFC mode, you must chip erase the
 * device and flash both the UICR and enter system flash image to the device.
 * 
 * This calls only does anything on nRF52840 devices, but compiles on all platforms. The
 * disableNFC() function is a no-op, and checkEnableNFC() always returns false on other platforms.
 */
class NFC_UICR_RK {
public:
    /**
     * @brief Disable NFC it was previously enabled
     * 
     * If it was enabled, the state will be set to disabled and the device will reboot.
     * 
     * If NFC was already disabled, the function will return immediately so your code can continue executing.
     * 
     * Note: You can only reenable NFC by chip erase and reprogram with a SWD programmer!
     */
    static void disableNFC();

    /**
     * @brief Returns true if NFC is enabled in the UICR bytes
     * 
     * @return true 
     * @return false 
     */
    static bool checkEnableNFC();
};



#endif // __NFC_UICR_RK_H

