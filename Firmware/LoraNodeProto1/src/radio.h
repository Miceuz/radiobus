#ifndef __RADIO_H
#define __RADIO_H

#include <lmic.h>

void on_tx_start();
void on_tx_complete(uint8_t dataLen, uint8_t dataBeg, uint8_t *frame);
void lora_init();
void lora_send(uint8_t *payload, uint8_t size);
void lora_adjust_time();

void lora_save_state();
void lora_restore_state();
bool is_lora_tx_path_busy();

#endif
