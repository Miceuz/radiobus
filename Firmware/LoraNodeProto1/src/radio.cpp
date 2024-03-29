#include "radio.h"
#include "hal/hal.h"
#include "pins.h"
#include <Arduino.h>

//--- DEVEUI is used in both OTA and ABP activation modes

// This should also be in little endian format, see above.
static const u1_t PROGMEM DEVEUI[8] = {0xF1, 0xE6, 0x04, 0xD0,
                                       0x7E, 0xD5, 0xB3, 0x70};
void os_getDevEui(u1_t *buf) { memcpy_P(buf, DEVEUI, 8); }

//--- APPEUI and APPKEY are used in OTA activation mode only

// This EUI must be in little-endian format, so least-significant-byte
// first. When copying an EUI from ttnctl output, this means to reverse
// the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3,
// 0x70.
static const u1_t PROGMEM APPEUI[8] = {0x67, 0x29, 0x02, 0xD0,
                                       0x7E, 0xD5, 0xB3, 0x70};
void os_getArtEui(u1_t *buf) { memcpy_P(buf, APPEUI, 8); }

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from the TTN console can be copied as-is.
static const u1_t PROGMEM APPKEY[16] = {0xB7, 0xF1, 0x29, 0x67, 0xA8, 0x49,
                                        0x4F, 0xDB, 0x50, 0xA5, 0xAB, 0xC3,
                                        0xCC, 0x40, 0xD1, 0xFA};
void os_getDevKey(u1_t *buf) { memcpy_P(buf, APPKEY, 16); }

//--- DEVADDR, NWKSKEY, APPSKEY are used ABP activation mode only

// LoRaWAN end-device address (DevAddr)
// See http://thethingsnetwork.org/wiki/AddressSpace
// The library converts the address to network byte order as needed, so this
// should be in big-endian (aka msb) too.
static const u4_t DEVADDR =
    0x260BB97E; // <-- Change this address for every node!

// LoRaWAN AppSKey, application session key
// This should also be in big-endian (aka msb).
static const u1_t PROGMEM APPSKEY[16] = {0x84, 0x26, 0xBF, 0xF6, 0x5F, 0x7D,
                                         0x17, 0x58, 0x32, 0x92, 0x1B, 0xD6,
                                         0x56, 0x11, 0x89, 0x0F};

// LoRaWAN NwkSKey, network session key
// This should be in big-endian (aka msb).
static const u1_t NWKSKEY[16] = {0xEA, 0xF7, 0xE8, 0xEB, 0x1A, 0x24,
                                 0x83, 0x90, 0x81, 0x31, 0x95, 0x60,
                                 0xF8, 0xD9, 0xE8, 0x39};

const lmic_pinmap lmic_pins = {
    .nss = RFM_NSS,
    .rxtx = PIN_RADIO_BAND_SEL,
    .rst = RFM_RST,
    .dio = {RFM_IRQ, RFM_DIO01, RFM_DIO02},
    .rxtx_rx_active = 1,
    .rssi_cal = 0,
    .spi_freq = 8000000,
};

lmic_t lmic_state;
bool lmic_state_saved = false;
void lora_save_state() {
  memcpy(&lmic_state, &LMIC, sizeof(lmic_t));
  lmic_state_saved = true;
}
void lora_restore_state() {
  if (lmic_state_saved) {
    memcpy(&LMIC, &lmic_state, sizeof(lmic_t));
  }
}

bool is_lora_tx_path_busy() {
  return LMIC.opmode & (OP_POLL | OP_TXDATA | OP_JOINING | OP_TXRXPEND);
}

void lora_init() {
  // LMIC init.
  os_init();
  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();

  // This example uses ABP activation mode, thus this initialization applies to
  // ABP. Look into LMIC library examples for how to set up OTA mode.
  LMIC_setSession(0x13, DEVADDR, (xref2u1_t)NWKSKEY, (xref2u1_t)APPSKEY);

  // Set up the channels used by the Things Network, which corresponds
  // to the defaults of most gateways. Without this, only three base
  // channels from the LoRaWAN specification are used, which certainly
  // works, so it is good for debugging, but can overload those
  // frequencies, so be sure to configure the full frequency range of
  // your network here (unless your network autoconfigures them).
  // Setting up channels should happen after LMIC_setSession, as that
  // configures the minimal channel set.

#if CFG_LMIC_EU_like
  LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7),
                    BAND_CENTI); // g-band
  LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7B),
                    BAND_CENTI); // g-band
  LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7),
                    BAND_CENTI); // g-band
  LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7),
                    BAND_CENTI); // g-band
  LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7),
                    BAND_CENTI); // g-band
  LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7),
                    BAND_CENTI); // g-band
  LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7),
                    BAND_CENTI); // g-band
  LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7),
                    BAND_CENTI); // g-band
#else
#if CFG_LMIC_US_like
  LMIC_selectSubBand(1);
#endif
#endif

  // Disable link check validation
  LMIC_setLinkCheckMode(0);
  // Disable adaptive data rate
  LMIC_setAdrMode(0);
  // TTN uses SF9 for its RX2 window.
#if CFG_LMIC_EU_like
  LMIC.dn2Dr = DR_SF9;
#endif
  // Set data rate and transmit power for uplink
#if CFG_LMIC_EU_like
  LMIC_setDrTxpow(DR_SF12, 14);
#endif
#if CFG_LMIC_US_like
  LMIC_setDrTxpow(DR_SF10, 14);
#endif

  // wait until LMIC is done initializing
  while (!hal_can_sleep()) {
    os_runloop_once();
  }
}

void lora_send(uint8_t *payload, uint8_t size) {
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND) {
    Serial.println(F("OP_TXRXPEND, not sending"));
  } else {
    //(port, payload, payload size, ack)
    LMIC_setTxData2(1, payload, size, 0);
  }
}

void lora_adjust_time() {
#if CFG_LMIC_EU_like
  LMIC.bands[BAND_MILLI].avail = os_getTime();
  LMIC.bands[BAND_CENTI].avail = os_getTime();
  LMIC.bands[BAND_DECI].avail = os_getTime();
#endif
}

void onEvent(ev_t ev) {
  // Serial.print(os_getTime());
  // Serial.print(": ");
  switch (ev) {
  case EV_SCAN_TIMEOUT:
    // Serial.println(F("EV_SCAN_TIMEOUT"));
    break;
  case EV_BEACON_FOUND:
    // Serial.println(F("EV_BEACON_FOUND"));
    break;
  case EV_BEACON_MISSED:
    // Serial.println(F("EV_BEACON_MISSED"));
    break;
  case EV_BEACON_TRACKED:
    // Serial.println(F("EV_BEACON_TRACKED"));
    break;
  case EV_JOINING:
    // Serial.println(F("EV_JOINING"));
    break;
  case EV_JOINED:
    // Serial.println(F("EV_JOINED"));
    // {
    //   u4_t netid = 0;
    //   devaddr_t devaddr = 0;
    //   u1_t nwkKey[16];
    //   u1_t artKey[16];
    //   LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);
    //   Serial.print("netid: ");
    //   Serial.println(netid, DEC);
    //   Serial.print("devaddr: ");
    //   Serial.println(devaddr, HEX);
    //   Serial.print("artKey: ");
    //   for (uint8_t i = 0; i < sizeof(artKey); ++i) {
    //     if (i != 0)
    //       Serial.print("-");
    //     Serial.print(artKey[i], HEX);
    //   }
    //   Serial.println("");
    //   Serial.print("nwkKey: ");
    //   for (uint8_t i = 0; i < sizeof(nwkKey); ++i) {
    //     if (i != 0)
    //       Serial.print("-");
    //     Serial.print(nwkKey[i], HEX);
    //   }
    //   Serial.println("");
    // }

    // Disable link check validation (automatically enabled
    // during join, but because slow data rates change max TX
    // size, we don't use it in this example.
    LMIC_setLinkCheckMode(0);
    break;
  case EV_JOIN_FAILED:
    // Serial.println(F("EV_JOIN_FAILED"));
    break;
  case EV_REJOIN_FAILED:
    // Serial.println(F("EV_REJOIN_FAILED"));
    break;
  case EV_TXCOMPLETE:
    // Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
    // if (LMIC.txrxFlags & TXRX_ACK)
    //   Serial.println(F("Received ack"));
    // if (LMIC.dataLen) {
    //   Serial.println(F("Received "));
    //   Serial.println(LMIC.dataLen);
    //   Serial.println(F(" bytes of payload"));
    // }
    on_tx_complete(LMIC.dataLen, LMIC.dataBeg, LMIC.frame);

    break;
  case EV_LOST_TSYNC:
    // Serial.println(F("EV_LOST_TSYNC"));
    break;
  case EV_RESET:
    // Serial.println(F("EV_RESET"));
    break;
  case EV_RXCOMPLETE:
    // data received in ping slot
    // Serial.println(F("EV_RXCOMPLETE"));
    break;
  case EV_LINK_DEAD:
    // Serial.println(F("EV_LINK_DEAD"));
    break;
  case EV_LINK_ALIVE:
    // Serial.println(F("EV_LINK_ALIVE"));
    break;
  /*
  || This event is defined but not used in the code. No
  || point in wasting codespace on it.
  ||
  || case EV_SCAN_FOUND:
  ||    Serial.println(F("EV_SCAN_FOUND"));
  ||    break;
  */
  case EV_TXSTART:
    // Serial.println(F("EV_TXSTART"));
    on_tx_start();
    break;
  case EV_RXSTART:
    // Serial.println(F("EV_RXSTART"));
    break;
  default:
    // Serial.print(F("Unknown event: "));
    // Serial.println((unsigned)ev);
    // lora_init();
    // on_tx_complete(LMIC.dataLen, LMIC.dataBeg, LMIC.frame);
    break;
  }
}
