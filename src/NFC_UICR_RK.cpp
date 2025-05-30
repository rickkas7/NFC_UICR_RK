#include "NFC_UICR_RK.h"

#if HAL_PLATFORM_NRF52840

#include "nrf_sdm.h"


// [static]
void NFC_UICR_RK::disableNFC() {
    if (!checkEnableNFC()) {
        // Already disabled
        return;
    }

    uint8_t softdevice_enabled = 0;
    sd_softdevice_is_enabled(&softdevice_enabled);
    if (softdevice_enabled) {
        sd_softdevice_disable();
    }

    // Perform UICR modification in an atomic block to ensure thread safety
    ATOMIC_BLOCK() {
        // Enable write access to NVMC
        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
            // Wait for NVMC to become ready
        }

        // Clear NFC protection bits to configure pins as GPIO
        NRF_UICR->NFCPINS &= ~UICR_NFCPINS_PROTECT_Msk;

        // Wait until the operation is complete
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
            // Wait for NVMC to become ready
        }

        // Disable write access to NVMC
        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
            // Wait for NVMC to become ready
        }
    }

    // Reset the system to apply changes
    System.reset();
}

// [static]
bool NFC_UICR_RK::checkEnableNFC() {

    return (NRF_UICR->NFCPINS & UICR_NFCPINS_PROTECT_Msk) != 0;
}
  
#else // HAL_PLATFORM_NRF52840

// [static]
void NFC_UICR_RK::disableNFC() {
    return;
}

// [static]
bool NFC_UICR_RK::checkEnableNFC() {

    return false;
}
#endif // HAL_PLATFORM_NRF52840