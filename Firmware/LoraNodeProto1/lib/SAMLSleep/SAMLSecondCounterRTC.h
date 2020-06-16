#ifndef _SAML_SECOND_COUNTER_RTC_H
#define _SAML_SECOND_COUNTER_RTC_H

#include <sam.h>

class SAMLSecondCounterRTC {
public:
  SAMLSecondCounterRTC(){};
  void begin() {
    if (started)
      return;
    /* Turn on the digital interface clock */
    MCLK->APBAMASK.reg |= MCLK_APBAMASK_RTC;
    // MCLK->APBAMASK.bit.RTC_ = 1;
    /* Select RTC clock: 1.024kHz from 32KHz ULP oscillator  */
    OSC32KCTRL->RTCCTRL.bit.RTCSEL = OSC32KCTRL_RTCCTRL_RTCSEL_ULP1K_Val;

    /* Reset module to hardware defaults. */
    // reset();
    /* Setup and Enable. */
    RTC->MODE0.CTRLA.reg =
        RTC_MODE0_CTRLA_ENABLE | RTC_MODE0_CTRLA_MODE_COUNT32 |
        RTC_MODE0_CTRLA_COUNTSYNC | RTC_MODE0_CTRLA_MATCHCLR |
        RTC_MODE0_CTRLA_PRESCALER_DIV1024; // one second
    waitSync();
    started = 1;
  };

  void reset() {
    disable(); // before soft-reset
    RTC->MODE0.CTRLA.reg |= RTC_MODE0_CTRLA_SWRST;
    while (RTC->MODE0.SYNCBUSY.reg || RTC->MODE0.CTRLA.bit.SWRST) {
    }
  };

  void disable() {
    waitSync();
    /* Disbale interrupt */
    RTC->MODE0.INTENCLR.reg = RTC_MODE0_INTENCLR_MASK;
    /* Clear interrupt flag */
    RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_MASK;
    /* Disable RTC module. */
    RTC->MODE0.CTRLA.reg &= ~RTC_MODE0_CTRLA_ENABLE;
    waitSync();
  };

  void set(uint32_t value) {
    if (0 == started)
      begin();
    waitSync();
    RTC->MODE0.COUNT.reg = value;
    waitSync();
  };

  uint32_t get() {
    if (0 == started)
      begin();
    waitSync();
    return RTC->MODE0.COUNT.reg;
  };

  void setCompare(uint32_t index, uint32_t value) {
    if (0 == started)
      begin();
    if (index > RTC_COMP32_NUM)
      abort();
    waitSync();
    RTC->MODE0.COMP[index].reg = value;
    waitSync();
  };

  uint32_t getCompare(uint32_t index) {
    if (0 == started)
      begin();
    if (index > RTC_COMP32_NUM)
      abort();
    waitSync();
    return RTC->MODE0.COMP[index].reg;
  };

  void interruptAfter(uint32_t seconds) {
    NVIC_DisableIRQ(RTC_IRQn);
    setCompare(0, get() + seconds);
    NVIC_ClearPendingIRQ(RTC_IRQn);
    NVIC_SetPriority(RTC_IRQn, 0);
    RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_MASK;
    NVIC_EnableIRQ(RTC_IRQn);
    RTC->MODE0.INTENSET.bit.CMP0 = 1;
  }

private:
  uint32_t started = 0;
  void waitSync() {
    while (RTC->MODE0.SYNCBUSY.reg) {
    }
  };

  void abort(void) {
    while (1) {
    }
  };
};

// void RTC_Handler() { RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_MASK; }
#endif