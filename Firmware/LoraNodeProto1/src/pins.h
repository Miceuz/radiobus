#define DE PIN_RS485_DE
#define RE PIN_RS485_RE
#define LED_WAN PIN_LED_WAN
#define LED_SENSOR PIN_LED_SENS
#define LED_BATT PIN_LED_BATT
#define RS_485_EN PIN_RS485_EN
#define I2C_EN PIN_I2C_EN
#define TEST_BUTTON PIN_TEST_BUTTON

#define RFM_NSS PIN_RADIO_CS
//#define RFM_RST PIN_RADIO_RST
//???
#define RFM_IRQ PIN_RADIO_DIO0
#define RFM_DIO01 PIN_RADIO_DIO1

#define i2cEnable() digitalWrite(I2C_EN, LOW)
#define i2cDisable() digitalWrite(I2C_EN, HIGH)
#define rs485Enable() digitalWrite(RS_485_EN, HIGH)
#define rs485Disable() digitalWrite(RS_485_EN, LOW)
#define rs485Sleep()                                                           \
  digitalWrite(RE, HIGH);                                                      \
  digitalWrite(DE, LOW)
#define rs485DriverEnable()                                                    \
  digitalWrite(RE, HIGH);                                                      \
  digitalWrite(DE, HIGH)
#define rs485ReaderEnable()                                                    \
  digitalWrite(DE, LOW);                                                       \
  digitalWrite(RE, LOW)
