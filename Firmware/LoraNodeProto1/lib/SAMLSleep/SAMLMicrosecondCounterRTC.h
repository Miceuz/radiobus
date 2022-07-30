#ifndef _SAML_MICROSECOND_COUNTER_RTC_H
#define _SAML_MICROSECOND_COUNTER_RTC_H

#include "SAMLCounterRTC.h"
#include <sam.h>

class SAMLMicrosecondCounterRTC : public SAMLCounterRTC {
public:
  SAMLMicrosecondCounterRTC() {
    rtc_sel = OSC32KCTRL_RTCCTRL_RTCSEL_ULP32K_Val;
    prescaler = RTC_MODE0_CTRLA_PRESCALER_DIV1;
  };
};

#endif