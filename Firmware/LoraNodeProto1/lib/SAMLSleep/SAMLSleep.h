#ifndef _SAML_SLEEP_H
#define _SAML_SLEEP_H
#include <Arduino.h>

#include "SAMLSecondCounterRTC.h"
#include <sam.h>
class SAMLSleep {
  SAMLSecondCounterRTC rtc;

public:
  SAMLSleep(){};

  enum class sleep_mode_e {
    SLEEP_MODE_IDLE = PM_SLEEPCFG_SLEEPMODE_IDLE,
    SLEEP_MODE_STANDBY = PM_SLEEPCFG_SLEEPMODE_STANDBY,
    SLEEP_MODE_BACKUP = PM_SLEEPCFG_SLEEPMODE_BACKUP,
    SLEEP_MODE_OFF = PM_SLEEPCFG_SLEEPMODE_OFF,
  };

  void begin() {
    rtc.begin();
    started = 1;
  }

  // Used for the CPU/APB clocks. Runs at 48MHz.
#define GENERIC_CLOCK_GENERATOR_MAIN (0u)
  // Enter in sleep in sleep_mode and wakeup after seconds
  void sleep(uint32_t seconds, sleep_mode_e sleep_mode) {
    if (0 == started) {
      begin();
    }

    // Errata: 15463
    // In Standby Sleep mode when the ADC is in free-running mode
    // (CTRLC.FREERUN=1) and the RUNSTDBY bit is set to 0 (CTRLA.RUNSTDBY=0),
    // the ADC keeps requesting its generic clock.
    // Stop the free-running mode (CTRLC.FREERUN=0) before entering Standby
    // Sleep mode.

    ADC->CTRLC.bit.FREERUN = 0;

    // Reference:14827, MATH100-7
    // When TRNG is disabled, some internal logic could continue to operate
    // causing an over consumption.
    // Workaround Disable the TRNG module twice:

    TRNG->CTRLA.reg = 0;
    TRNG->CTRLA.reg = 0;

    // Errata: 14539
    // If the PM.STDBYCFG.VREGSMOD field is set to 2 (low-power configuration),
    // the oscillator source driving the GCLK_MAIN clock will still be running
    // in Standby mode causing extra consumption.
    // Before entering Standby mode, switch the GCLK_MAIN to the OSCULP32K
    // clock. After wake-up, switch back to the GCLK_MAIN clock.

    // MCLK->APBDMASK.bit.AC_ = 0;
    // MCLK->APBDMASK.bit.ADC_ = 0;
    // MCLK->APBDMASK.bit.CCL_ = 0;
    // MCLK->APBDMASK.bit.EVSYS_ = 0;
    // MCLK->APBDMASK.bit.PTC_ = 0;
    // MCLK->APBDMASK.bit.SERCOM5_ = 0;
    // MCLK->APBDMASK.bit.TC4_ = 0;

    // MCLK->AHBMASK.bit.DMAC_ = 0;
    // MCLK->AHBMASK.bit.DSU_ = 0;
    // MCLK->AHBMASK.bit.PAC_ = 0;
    // MCLK->APBAMASK.bit.PORT_ = 0;
    // MCLK->APBAMASK.bit.RSTC_ = 0;
    // MCLK->APBAMASK.bit.SUPC_ = 0;
    // MCLK->APBCMASK.bit.AES_ = 0;
    // MCLK->APBCMASK.bit.DAC_ = 0;
    // MCLK->APBCMASK.bit.SERCOM0_ = 0;
    // MCLK->APBCMASK.bit.SERCOM1_ = 0;
    // MCLK->APBCMASK.bit.SERCOM2_ = 0;
    // MCLK->APBCMASK.bit.SERCOM3_ = 0;
    // MCLK->APBCMASK.bit.SERCOM4_ = 0;
    // MCLK->APBCMASK.bit.TC0_ = 0;
    // MCLK->APBCMASK.bit.TC1_ = 0;
    // MCLK->APBCMASK.bit.TC2_ = 0;
    // MCLK->APBCMASK.bit.TC3_ = 0;
    // MCLK->APBCMASK.bit.TCC0_ = 0;
    // MCLK->APBCMASK.bit.TCC1_ = 0;
    // MCLK->APBCMASK.bit.TCC2_ = 0;
    // MCLK->APBCMASK.bit.TRNG_ = 0;

    // MCLK->APBEMASK.bit.PAC_ = 0;

    // for (int i = 4; i < 35; i++) {
    //   if (0 != GCLK->PCHCTRL[i].reg) {
    //     GCLK->PCHCTRL[i].reg = 0;
    //     while ((GCLK->PCHCTRL[i].reg & GCLK_PCHCTRL_CHEN) !=
    //     GCLK_PCHCTRL_CHEN)
    //       ;
    //   }
    // }

    // USB pins set to pullups
    PORT->Group[0].OUTSET.reg = (uint32_t)(1 << PIN_PA24G_USB_DM);
    PORT->Group[0].PINCFG[PIN_PA24G_USB_DM].reg =
        (PORT_PINCFG_PULLEN | PORT_PINCFG_INEN);
    PORT->Group[0].OUTSET.reg = (uint32_t)(1 << PIN_PA25G_USB_DP);
    PORT->Group[0].PINCFG[PIN_PA25G_USB_DP].reg =
        (PORT_PINCFG_PULLEN | PORT_PINCFG_INEN);

    GCLK->GENCTRL[GENERIC_CLOCK_GENERATOR_MAIN].reg =
        (GCLK_GENCTRL_DIV(2) | GCLK_GENCTRL_SRC_OSCULP32K | GCLK_GENCTRL_IDC |
         GCLK_GENCTRL_GENEN);
    syncClocks();

    PM->INTFLAG.bit.PLRDY = 1;
    PM->PLCFG.bit.PLSEL = PM_PLCFG_PLSEL_PL0_Val;
    while (PM->INTFLAG.bit.PLRDY)
      ;
    PM->INTFLAG.bit.PLRDY = 1;

    PM->STDBYCFG.bit.VREGSMOD = PM_STDBYCFG_VREGSMOD_LP_Val;
    PM->STDBYCFG.bit.BBIASLP = PM_STDBYCFG_BBIASLP(2);
    PM->STDBYCFG.bit.BBIASHS = PM_STDBYCFG_BBIASHS(2);
    PM->STDBYCFG.bit.BBIASPP = PM_STDBYCFG_BBIASPP(2);

    rtc.interruptAfter(seconds);
    setMode(sleep_mode);
    sleep();

    PM->INTFLAG.bit.PLRDY = 1;
    PM->PLCFG.bit.PLSEL =
        PM_PLCFG_PLSEL_PL2_Val; // must set to highest performance level
    while (PM->INTFLAG.bit.PLRDY)
      ;
    PM->INTFLAG.bit.PLRDY = 1;

    GCLK->GENCTRL[GENERIC_CLOCK_GENERATOR_MAIN].reg =
        (GCLK_GENCTRL_DIV(2) | GCLK_GENCTRL_SRC_DPLL96M | GCLK_GENCTRL_IDC |
         GCLK_GENCTRL_GENEN);
    syncClocks();
  };

  // void set_backup(int n_values, ...) {
  //   n_values &= 3;
  //   va_list list;
  //   va_start(list, n_values);
  //   for (int i = 0; i < n_values; i++)
  //     RTC->MODE0.GP[i].reg = va_arg(list, uint32_t);
  //   va_end(list);
  // }

  // /*
  //     Restore max uint32[ 4 ] values from rtc ram
  //     rtc.get_backup(1, &val_1);
  //       ...
  //     rtc.get_backup(4, &val_1, &val_2, &val_3, &val_4);
  // */
  // void get_backup(int n_values, ...) {
  //   n_values &= 3;
  //   va_list list;
  //   va_start(list, n_values);
  //   for (int i = 0; i < n_values; i++) {
  //     uint32_t *p = va_arg(list, uint32_t *);
  //     if (p)
  //       *p = RTC->MODE0.GP[i].reg;
  //   }
  //   va_end(list);
  // }

private:
  uint32_t started = 0;

  static inline void setMode(const enum sleep_mode_e sleep_mode) {
    PM->SLEEPCFG.reg = static_cast<uint8_t>(sleep_mode);
    while (PM->SLEEPCFG.reg != static_cast<uint8_t>(sleep_mode)) {
      // wait
    }
  };

  static inline void sleep(void) /* DO NOT ENTER FAST ! */
  {
    __DSB();
    __WFI();
  };

  void syncClocks(void) {
    while (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_MASK) {
      // wait
    }
  }
};

#endif