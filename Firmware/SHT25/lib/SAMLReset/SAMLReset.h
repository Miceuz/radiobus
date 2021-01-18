#ifndef _SAML_RESET_H
#define _SAML_RESET_H

#include <sam.h>

enum reset_cause_e {
  /** The system was last reset by a backup reset */
  RESET_CAUSE_BACKUP = RSTC_RCAUSE_BACKUP,
  /** The system was last reset by a software reset */
  RESET_CAUSE_SOFTWARE = RSTC_RCAUSE_SYST,
  /** The system was last reset by the watchdog timer */
  RESET_CAUSE_WDT = RSTC_RCAUSE_WDT,
  /** The system was last reset because the external reset line was pulled low
   */
  RESET_CAUSE_EXTERNAL_RESET = RSTC_RCAUSE_EXT,
#if SAML21 || SAMR30 || (SAMR34)
  /** The system was last reset by the BOD33. */
  RESET_CAUSE_BOD33 = RSTC_RCAUSE_BOD33,
  /** The system was last reset by the BOD12 */
  RESET_CAUSE_BOD12 = RSTC_RCAUSE_BOD12,
#else
  /** The system was last reset by the BOD VDD. */
  RESET_CAUSE_BOD33 = RSTC_RCAUSE_BODVDD,
  /** The system was last reset by the BOD CORE. */
  RESET_CAUSE_BOD12 = RSTC_RCAUSE_BODCORE,
#endif
  /** The system was last reset by the POR (Power on reset). */
  RESET_CAUSE_POR = RSTC_RCAUSE_POR,
};

static inline enum reset_cause_e get_reset_cause(void) {
  return (enum reset_cause_e)RSTC->RCAUSE.reg;
}

#endif