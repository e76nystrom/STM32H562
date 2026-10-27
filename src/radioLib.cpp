//
// Created by Eric Nystrom on 10/21/26.
//

#include <RadioLib.h>
//#include "main.h"
#include "spi.h"
#include "Hal.h"
#include "stmHal.h"

StmHal hal(&hspi2);
Module mod(&hal, PIN_LORA_CS, PIN_LORA_DIO1, PIN_LORA_RST, PIN_LORA_BUSY);
SX1262 radio(&mod);

volatile bool rxFlag = false;
static void onRx() { rxFlag = true; }

void lora_init()
{
 int state = radio.begin(915.0, 125.0, 9, 7, RADIOLIB_SX126X_SYNC_WORD_PRIVATE, 14);
 // check state == RADIOLIB_ERR_NONE
 radio.setDio1Action(onRx);
 radio.startReceive();
}

void lora_poll()
{
 if (rxFlag)
 {
  rxFlag = false;
  uint8_t buf[255];
  int state = radio.readData(buf, sizeof(buf));
  size_t len = radio.getPacketLength();
  // use buf/len, then
  radio.startReceive();
 }
}

// Transmit (blocking): radio.transmit(data, len);
// Non-blocking: radio.startTransmit(data, len), then wait for DIO1
