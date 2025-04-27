#ifndef __NFC_UICR_RK_H
#define __NFC_UICR_RK_H

// Github: https://github.com/rickkas7/NFC_UICR_RK
// License: MIT

#include "Particle.h"

/**
 * @brief Class for updating the nRF52840 UICR bytes to enable or disable NFC
 * 
 * You will typically call NFC_UICR_RK::disable() to disable NFC on devices that have it enabled but you
 * want to use the pins as GPIO. The E404X and Monitor One already have it disabled from the factory.
 * 
 * If the state of NFC is different than what you've enabled or disabled it will be updated. There
 * are caveats to this:
 * - You should make the call as close as possible to the beginning of setup()
 * - Use SYSTEM_MODE(SEMI_AUTOMATIC) 
 * - Particle.connect() any time after calling one of the functions, or at the end of setup()
 * - Don't enable BLE before calling one of these functions
 * 
 * The reason is that if the UICR bits need to be updated, SoftDevice (BLE) needs to be disabled. It
 * will also System.reset() after updating the bits. Because of this, there is a possibility that the code
 * after enable() or disable() will never be executed.
 * 
 * This calls only does anything on nRF52840 devices, but compiles on all platforms. The enable()
 * and disable() functions are no-op, and checkEnableNFC() always returns true on other platforms.
 * 
 */
class NFC_UICR_RK {
public:
    /**
     * @brief Disable NFC it was previously enabled
     * 
     * If it was enabled, the state will be set to disabled and the device will reboot.
     * 
     * If NFC was already disabled, the function will return immediately so your code can continue executing.
     */
    static void disable();

    /**
     * @brief Enable NFC it was previously disbled
     * 
     * If it was disabled, the state will be set to enabled and the device will reboot.
     * 
     * If NFC was already enabled, the function will return immediately so your code can continue executing.
     */
    static void enable();

    /**
     * @brief Returns true if NFC is enabled in the UICR bytes
     * 
     * @return true 
     * @return false 
     */
    static bool checkEnableNFC();

private:
    /**
     * @brief Used internally by disable() or enable(). You should not call this directly.
     * 
     * @param enableNFC true to enable NFC, false to use the NFC pins as GPIO
     * 
     * This method does not check to see if the state is already set! That check is in
     * enable() and disable() which is why you should not call this directly.
     */
    static void setEnableAndReset(bool enableNFC);

};



#endif // __NFC_UICR_RK_H

