#include "STM32LowPower.h"

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
  //pinMode(BATT_MON, OUTPUT);

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
  // digitalWrite(BATT_MON, LOW);

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


  LowPower.begin();
  Serial.begin(9600);

  digitalWrite(RS485_ENABLE, HIGH);

  pinMode(TEST_BUTTON, INPUT_PULLUP);
  attachInterrupt(TEST_BUTTON, onButtonPress, FALLING);
  while(!Serial);
}

/**
 * @returns battery voltage in millivolts
*/
uint32_t BatteryVoltage() {
  analogReadResolution(12);
  return analogRead(A8) * 3300 * 4 / 4095;
}

void loop() {
  Serial.begin(9600);
  Serial.println("Batt voltage:");
  Serial.println(BatteryVoltage());
  Serial.flush();
  digitalWrite(LED, HIGH);
  LowPower.deepSleep(100);
  digitalWrite(LED, LOW);
  LowPower.deepSleep(5000);
}

void onButtonPress() {
  Serial.begin(9600);
  Serial.println("Button!");
  Serial.flush();
}