#include <Arduino.h>
// #include "STM32LowPower.h"
#include <STM32LoRaWAN.h>
#include <BSP/timer.h>

#define RS485_RE PB5
#define RS485_DE PA1
#define RS485_TX PB6
#define RS485_RX PB7
#define I2C_ENABLE PA15
#define RS485_ENABLE PB3
#define UART_TX PA2
#define UART_RX PA3
#define I2C_SDA PA11
#define I2C_SCL PA12
#define TEST_BUTTON PB12
#define A1 PA10
#define BATT_MON PB4 

#define SS PA4
#define SCK PA5
#define MISO PA6
#define MOSI PA7
#define LED PA8
#define LED2 PA8
#define LED1 PA9
#define LED3 PB2
#define TP2 PA0
#define SWDCLK PA14
#define SWDIO PA13


STM32LoRaWAN modem;

extern "C" void print_this(HAL_StatusTypeDef status);

void SleepFor(int32_t milliseconds);

TimerEvent_t SleepTimer;
void onSleepTimer(void *context) {
  // just wakeupt
}
// stm32flash -i -dtr,-rts,:rts,-dtr,dtr, /dev/ttyUSB0
void setup() {
  pinMode(LED, OUTPUT);

  pinMode(RS485_RE, OUTPUT);
  pinMode(RS485_DE, OUTPUT);
  pinMode(RS485_TX, OUTPUT);
  pinMode(RS485_RX, OUTPUT);
  pinMode(I2C_ENABLE, OUTPUT);
  pinMode(RS485_ENABLE, OUTPUT);
  pinMode(UART_TX, OUTPUT);
  pinMode(UART_RX, OUTPUT);
  pinMode(I2C_SDA, OUTPUT);
  pinMode(I2C_SCL, OUTPUT);
  pinMode(TEST_BUTTON, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(BATT_MON, OUTPUT);

  pinMode(SS, OUTPUT);
  pinMode(SCK, OUTPUT);
  pinMode(MISO, OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(TP2, OUTPUT);
  pinMode(SWDCLK, OUTPUT);
  pinMode(SWDIO, OUTPUT);

  digitalWrite(RS485_RE, HIGH);
  digitalWrite(RS485_DE, LOW);
  digitalWrite(RS485_TX, LOW);
  digitalWrite(RS485_RX, LOW);
  digitalWrite(I2C_ENABLE, HIGH);
  digitalWrite(RS485_ENABLE, LOW);
  digitalWrite(UART_TX, LOW);
  digitalWrite(UART_RX, LOW);
  digitalWrite(I2C_SDA, LOW);
  digitalWrite(I2C_SCL, LOW);
  digitalWrite(TEST_BUTTON, HIGH);
  digitalWrite(A1, LOW);
  digitalWrite(BATT_MON, LOW);

  digitalWrite(SS, LOW);
  digitalWrite(SCK, LOW);
  digitalWrite(MISO, LOW);
  digitalWrite(MOSI, LOW);
  digitalWrite(LED, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED1, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(TP2, LOW);
  digitalWrite(SWDCLK, HIGH);
  digitalWrite(SWDIO, LOW);

  Serial.begin(9600);
  delay(2000);
  Serial.println("Start");

  TimerInit(&SleepTimer, onSleepTimer);


  modem.begin(EU868);
  modem.dataRate(DR_5);
  modem.power(0);
  modem.setADR(false);

  // bool connected =
  //     modem.joinOTAA(/* AppEui */ "0000000000000000",
  //                    /* AppKey */ "00000000000000000000000000000000",
  //                    /* DevEui */ "0000000000000000");
  bool connected =
      modem.joinABP(/* DevAddr */ "260BB97E",           /* NwkSKey */
                    "EAF7E8EB1A24839081319560F8D9E839", /* AppSKey */
                    "8426BFF65F7D175832921BD65611890F");

  if (connected) {
    Serial.println("Joined");
  } else {
    digitalWrite(LED, HIGH);
    Serial.println("Join failed");
    while (true) /* infinite loop */
      ;
  }
  SleepFor(5000);
}

void wait_for_idle() {
  while (modem.busy()) {
    // Call maintain() so the lora library can do any
    // pending background work too.
    modem.maintain();
    if(modem.busy()) {
      SleepFor(-1);
      modem.maintain();
    }
  }
}

void send_packet() {
  digitalWrite(LED2, HIGH);
  uint8_t payload[] = {0xde, 0xad, 0xbe, 0xef};
  modem.setPort(10);
  modem.beginPacket();
  modem.write(payload, sizeof(payload));
  if (modem.endPacketAsync() == sizeof(payload)) {
    Serial.println("Sent packet");
  } else {
    Serial.println("Failed to send packet");
  }

  wait_for_idle();
  Serial.begin(9600);
  if (modem.available()) {
    Serial.print("Received packet on port ");
    Serial.print(modem.getDownlinkPort());
    Serial.print(":");
    while (modem.available()) {
      uint8_t b = modem.read();
      Serial.print(" ");
      Serial.print(b >> 4, HEX);
      Serial.print(b & 0xF, HEX);
    }
    Serial.println();
  }
  digitalWrite(LED2, LOW);
}

static const unsigned long TX_INTERVAL = 60000; /* ms */
unsigned long last_tx = 0;

void SleepFor(int32_t milliseconds) {
  if(milliseconds > 0) {
    TimerSetValue(&SleepTimer, milliseconds);
    TimerStart(&SleepTimer);
  }
  Serial.end();
  pinMode(UART_RX, OUTPUT);
  pinMode(UART_TX, OUTPUT);
  digitalWrite(UART_RX, LOW);
  digitalWrite(UART_TX, LOW);
  HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
}

void loop() {
  Serial.begin(9600);
  // if (!last_tx || millis() - last_tx > TX_INTERVAL) {
    send_packet();
    last_tx = millis();
  // }
  // digitalWrite(LED, HIGH);
  // delay(100);
  // digitalWrite(LED, LOW);
  SleepFor(5000);
}
