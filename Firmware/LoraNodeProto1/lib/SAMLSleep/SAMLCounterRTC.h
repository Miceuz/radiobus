#ifndef _SAML_COUNTER_RTC_H
#define _SAML_COUNTER_RTC_H
#include <sam.h>

class SAMLCounterRTC {
public:
  SAMLCounterRTC(){};

  // 1k clock source and 1k prescaler gives us RTC tick 1 second long

  void interruptAfter(uint32_t ticks) {
    reset();
    begin();
    NVIC_DisableIRQ(RTC_IRQn);
    setCompare(0, get() + ticks);
    NVIC_ClearPendingIRQ(RTC_IRQn);
    NVIC_SetPriority(RTC_IRQn, 0);
    RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_MASK;
    NVIC_EnableIRQ(RTC_IRQn);
    RTC->MODE0.INTENSET.bit.CMP0 = 1;
  }

protected:
  uint32_t rtc_sel;
  uint32_t prescaler;
  void begin() {
    /* Turn on the digital interface clock */
    MCLK->APBAMASK.reg |= MCLK_APBAMASK_RTC;
    OSC32KCTRL->RTCCTRL.bit.RTCSEL = rtc_sel;

    /* Setup and Enable. */
    RTC->MODE0.CTRLA.reg =
        RTC_MODE0_CTRLA_ENABLE | RTC_MODE0_CTRLA_MODE_COUNT32 |
        RTC_MODE0_CTRLA_COUNTSYNC | RTC_MODE0_CTRLA_MATCHCLR | prescaler;
    waitSync();
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
    waitSync();
    RTC->MODE0.COUNT.reg = value;
    waitSync();
  };

  uint32_t get() {
    waitSync();
    return RTC->MODE0.COUNT.reg;
  };

  void setCompare(uint32_t index, uint32_t value) {
    if (index > RTC_COMP32_NUM)
      abort();
    waitSync();
    RTC->MODE0.COMP[index].reg = value;
    waitSync();
  };

  uint32_t getCompare(uint32_t index) {
    if (index > RTC_COMP32_NUM)
      abort();
    waitSync();
    return RTC->MODE0.COMP[index].reg;
  };

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