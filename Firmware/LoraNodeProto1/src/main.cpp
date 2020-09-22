#include "SAMLSleep.h"
#include "radio.h"
#include "wiring_private.h"
#include <Arduino.h>
#include <SPI.h>

SAMLSleep sleep;
void onTestButtonPressed();

void pinsToSleep() {
  pinPeripheral(0, PIO_OUTPUT);
  pinPeripheral(1, PIO_OUTPUT);
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);

  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(PIN_SPI1_MISO, OUTPUT);
  pinMode(PIN_SPI1_MOSI, OUTPUT);
  pinMode(PIN_SPI1_SCK, OUTPUT);
  pinMode(PIN_SPI1_SS, OUTPUT);

  pinMode(PIN_RADIO_CS, OUTPUT);
  pinMode(PIN_RADIO_RST, OUTPUT);
  pinMode(PIN_RADIO_DIO0, OUTPUT);
  pinMode(PIN_RADIO_DIO1, OUTPUT);
  pinMode(PIN_RADIO_DIO2, OUTPUT);
  pinMode(PIN_RADIO_DIO3, OUTPUT);
  pinMode(PIN_RADIO_DIO4, OUTPUT);
  pinMode(PIN_RADIO_DIO5, OUTPUT);

  pinMode(PIN_RS485_RE, OUTPUT);
  pinMode(PIN_RS485_DE, OUTPUT);
  pinMode(PIN_RS485_EN, OUTPUT);
  pinMode(PIN_I2C_EN, OUTPUT);
  pinMode(PIN_TEST_BUTTON, INPUT_PULLUP);
  pinMode(PIN_SERIAL1_RX, OUTPUT);
  pinMode(PIN_SERIAL1_TX, OUTPUT);
  pinMode(PIN_WIRE_SCL, OUTPUT);
  pinMode(PIN_WIRE_SDA, OUTPUT);

  digitalWrite(0, LOW);
  digitalWrite(1, LOW);
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);

  digitalWrite(A0, LOW);
  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
  digitalWrite(PIN_SPI1_MISO, LOW);
  digitalWrite(PIN_SPI1_MOSI, LOW);
  digitalWrite(PIN_SPI1_SCK, LOW);
  digitalWrite(PIN_SPI1_SS, LOW);

  digitalWrite(PIN_RADIO_CS, LOW);
  digitalWrite(PIN_RADIO_RST, HIGH);
  digitalWrite(PIN_RADIO_DIO0, LOW);
  digitalWrite(PIN_RADIO_DIO1, LOW);
  digitalWrite(PIN_RADIO_DIO2, LOW);
  digitalWrite(PIN_RADIO_DIO3, LOW);
  digitalWrite(PIN_RADIO_DIO4, LOW);
  digitalWrite(PIN_RADIO_DIO5, LOW);

  digitalWrite(PIN_RS485_RE, LOW);
  digitalWrite(PIN_RS485_DE, LOW);
  digitalWrite(PIN_RS485_EN, LOW);
  digitalWrite(PIN_I2C_EN, LOW);
  digitalWrite(PIN_SERIAL1_RX, LOW);
  digitalWrite(PIN_SERIAL1_TX, LOW);
  digitalWrite(PIN_WIRE_SCL, LOW);
  digitalWrite(PIN_WIRE_SDA, LOW);
  attachInterrupt(PIN_TEST_BUTTON, onTestButtonPressed, FALLING);
}

void setup() {
  pinMode(PIN_RADIO_SWITCH_PWR, OUTPUT);
  pinMode(PIN_RADIO_TXCO_PWR, OUTPUT);
  pinMode(PIN_RADIO_BAND_SEL, OUTPUT);

  digitalWrite(PIN_RADIO_SWITCH_PWR, HIGH);
  digitalWrite(PIN_RADIO_TXCO_PWR, HIGH);

  lora_init();
  LMIC_shutdown();
  delay(100);

  digitalWrite(PIN_RADIO_SWITCH_PWR, LOW);
  digitalWrite(PIN_RADIO_TXCO_PWR, LOW);
  digitalWrite(PIN_RADIO_BAND_SEL, LOW);

  delay(100);

  pinsToSleep();

  pinMode(PIN_LED_WAN, OUTPUT);
  pinMode(PIN_LED_SENS, OUTPUT);
  pinMode(PIN_LED_BATT, OUTPUT);

  digitalWrite(PIN_LED_WAN, HIGH);
  digitalWrite(PIN_LED_BATT, HIGH);
  digitalWrite(PIN_LED_SENS, HIGH);
  delay(300);
  digitalWrite(PIN_LED_WAN, LOW);
  digitalWrite(PIN_LED_BATT, LOW);
  digitalWrite(PIN_LED_SENS, LOW);

  // delay(500);
  // USB->DEVICE.CTRLA.bit.ENABLE = 0;
  //   GCLK->GENCTRL[GENERIC_CLOCK_GENERATOR_TIMERS].bit.GENEN = 0;
  GCLK->GENCTRL[GENERIC_CLOCK_GENERATOR_48MHz].bit.GENEN = 0;
  // #define GENERIC_CLOCK_GENERATOR_OSCULP32K (2u)
  //   GCLK->GENCTRL[GENERIC_CLOCK_GENERATOR_OSCULP32K].bit.GENEN = 0;
  // disabling this causes higher power consumption
  // GCLK->GENCTRL[GENERIC_CLOCK_GENERATOR_OSC_HS].bit.GENEN = 0;
  // OSCCTRL->OSC16MCTRL.bit.ENABLE = 0;

  // MCLK->APBDMASK.reg &= ~MCLK_APBDMASK_ADC;

  // delay(500);

  // Serial.begin(9600);
}

// // 0x10810228 - ATSAMR34J18B
// void loop() {
//   Serial.println(DSU->DID.reg, HEX);

//   Serial.print("processor: ");
//   Serial.println(DSU->DID.bit.PROCESSOR, HEX);
//   Serial.print("family: ");
//   Serial.println(DSU->DID.bit.FAMILY, HEX);
//   Serial.print("series: ");
//   Serial.println(DSU->DID.bit.SERIES, HEX);
//   Serial.print("die: ");
//   Serial.println(DSU->DID.bit.DIE, HEX);
//   Serial.print("revision: ");
//   Serial.println(DSU->DID.bit.REVISION, HEX);
//   Serial.print("devsel: ");
//   Serial.println(DSU->DID.bit.DEVSEL, HEX);
//   delay(1000);
// }

void loop() {
  delay(100);
  // digitalWrite(PIN_LED_SENS, LOW);
  pinsToSleep();
  sleep.sleep(2, SAMLSleep::sleep_mode_e::SLEEP_MODE_STANDBY);
  // digitalWrite(PIN_LED_BATT, HIGH);
  delay(10);
  // digitalWrite(PIN_LED_BATT, LOW);
}

void RTC_Handler() { RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_MASK; }

void onTestButtonPressed() {
  // just wakeup
}