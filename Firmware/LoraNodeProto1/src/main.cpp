#include "SAMLSleep.h"
#include "pins.h"
#include "radio.h"
#include "wiring_private.h"
#include <Arduino.h>
#include <SPI.h>

void onTestButtonPressed();
void read_sensors();
uint8_t setupPayload();

SAMLSleep sleep;

uint16_t batteryMillivolts = 0;

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL_S = 5;

bool loraTxInProgress = false;

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

  digitalWrite(PIN_RS485_RE, HIGH);
  digitalWrite(PIN_RS485_DE, LOW);
  digitalWrite(PIN_RS485_EN, LOW);
  digitalWrite(PIN_I2C_EN, HIGH);
  digitalWrite(PIN_SERIAL1_RX, LOW);
  digitalWrite(PIN_SERIAL1_TX, LOW);
  digitalWrite(PIN_WIRE_SCL, LOW);
  digitalWrite(PIN_WIRE_SDA, LOW);
  attachInterrupt(PIN_TEST_BUTTON, onTestButtonPressed, FALLING);
}

void setup() {
  Serial.begin(115200);

  pinsToSleep();

  pinMode(PIN_RADIO_SWITCH_PWR, OUTPUT);
  pinMode(PIN_RADIO_TXCO_PWR, OUTPUT);
  pinMode(PIN_RADIO_BAND_SEL, OUTPUT);

  digitalWrite(PIN_RADIO_SWITCH_PWR, HIGH);
  digitalWrite(PIN_RADIO_TXCO_PWR, HIGH);

  lora_init();

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

  read_sensors();
}

/*****************************************************************
  Add your sensor code in this function
******************************************************************/
void read_sensors() {
  digitalWrite(PIN_LED_SENS, HIGH);
  pinMode(A0, INPUT);
  i2cEnable();
  rs485Enable(); // enabling rs485 also enables resistive divider for battery
                 // measurement
  delay(50);     // allow voltage to stabilize

  analogRead(A0);
  analogRead(A0);
  analogRead(A0);

  batteryMillivolts = analogRead(A0) * 2 * 3300 / 1024;
  i2cDisable();
  rs485Disable();
  digitalWrite(PIN_LED_SENS, LOW);
}

/*********************************************************************
  Adjust PAYLOAD_LENGTH according to how many bytes you want to send
**********************************************************************/
#define PAYLOAD_LENGTH 5
// payload to send to TTN gateway
static uint8_t payload[PAYLOAD_LENGTH];
uint8_t setupPayload() {
  payload[0] = batteryMillivolts & 0x00FF;
  payload[1] = (batteryMillivolts >> 8) & 0x00FF;
  return 2; // return the length of the payload
}

void lora_sleep() {
  lora_save_state();
  LMIC_shutdown();

  digitalWrite(PIN_RADIO_SWITCH_PWR, LOW);
  digitalWrite(PIN_RADIO_TXCO_PWR, LOW);
  digitalWrite(PIN_RADIO_BAND_SEL, LOW);
}

void lora_wakeup() {
  digitalWrite(PIN_RADIO_SWITCH_PWR, HIGH);
  digitalWrite(PIN_RADIO_TXCO_PWR, HIGH);

  lora_init();
  lora_restore_state();
  lora_adjust_time();
}

void go_to_sleep() {
  pinsToSleep();
  sleep.sleep(TX_INTERVAL_S, SAMLSleep::unit_e::SECOND,
              SAMLSleep::sleep_mode_e::SLEEP_MODE_STANDBY);
  // sleep.sleep(TX_INTERVAL_S * 1000 * 1000 / 30,
  // SAMLSleep::unit_e::T30US,
  //             SAMLSleep::sleep_mode_e::SLEEP_MODE_STANDBY);
  // delay(TX_INTERVAL_S * 1000);
}

void on_tx_start() {
  // digitalWrite(LED_WAN, HIGH);
}

void on_tx_complete(uint8_t dataLen, uint8_t dataBeg, uint8_t *frame) {
  if (dataLen > 0) {
    uint8_t first_byte = frame[dataBeg];
    // to do process your downlink data here
  }

  loraTxInProgress = false;
  digitalWrite(LED_WAN, LOW);
}

void lora_send_payload() {
  lora_wakeup();
  uint8_t payload_length = setupPayload();
  lora_send(payload, payload_length);
  loraTxInProgress = true;

  Serial.println(EIC->ASYNCH.reg, BIN);

  uint32_t ts = millis();
  while (loraTxInProgress || is_lora_tx_path_busy()) {
    hal_sleep_reset();
    os_runloop_once();
    if (hal_can_sleep()) {
      ostime_t next_job_time = os_getNextJobDeadline();
      if (next_job_time > 0) {
        ostime_t ticks_to_job = next_job_time - os_getTime();
        uint32_t ms_to_job = ticks_to_job * US_PER_OSTICK / 1000;
        if (ms_to_job >= 1) {
          digitalWrite(PIN_RADIO_TXCO_PWR, LOW);
          digitalWrite(PIN_RADIO_BAND_SEL, LOW);
          digitalWrite(PIN_RADIO_SWITCH_PWR, LOW);

          sleep.sleep(ms_to_job * 1000 / 30, SAMLSleep::unit_e::T30US,
                      SAMLSleep::sleep_mode_e::SLEEP_MODE_STANDBY);
          digitalWrite(PIN_RADIO_TXCO_PWR, HIGH);
          digitalWrite(PIN_RADIO_SWITCH_PWR, HIGH);

          fast_forward_time(ms_to_job);
        }
      } else {
        // TODO
      }
    }
  }
  lora_sleep();
}

void loop() {
  read_sensors();
  lora_send_payload();
  go_to_sleep();
}

void RTC_Handler() { RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_MASK; }

void onTestButtonPressed() {
  // just wakeup
}