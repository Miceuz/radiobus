#ifndef _SAML_SECOND_COUNTER_RTC_H
#define _SAML_SECOND_COUNTER_RTC_H

#include "SAMLCounterRTC.h"
#include <sam.h>
class SAMLSecondCounterRTC : public SAMLCounterRTC {
public:
  SAMLSecondCounterRTC() {
    rtc_sel = OSC32KCTRL_RTCCTRL_RTCSEL_ULP1K_Val;
    prescaler = RTC_MODE0_CTRLA_PRESCALER_DIV1024;
  }

  // 1k clock source and 1k prescaler gives us RTC tick 1 second long
};
#endif