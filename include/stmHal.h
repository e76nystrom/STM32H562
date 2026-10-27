//
// Created by Eric Nystrom on 10/21/26.
//

#ifndef STM32H562_STMHAL_H
#define STM32H562_STMHAL_H
// stm_hal.h
#pragma once
#include "Hal.h"
#include "main.h"

enum { PIN_LORA_CS = 0, PIN_LORA_RST, PIN_LORA_BUSY, PIN_LORA_DIO1,
       PIN_LORA_RXEN, PIN_LORA_TXEN };

class StmHal : public RadioLibHal {
public:
 explicit StmHal(SPI_HandleTypeDef *spi);

 void init() override;
 void term() override;

 void pinMode(uint32_t pin, uint32_t mode) override;
 void digitalWrite(uint32_t pin, uint32_t value) override;
 uint32_t digitalRead(uint32_t pin) override;

 void attachInterrupt(uint32_t interruptNum, void (*interruptCb)(void), uint32_t mode) override;
 void detachInterrupt(uint32_t interruptNum) override;

 void delay(unsigned long ms) override;
 void delayMicroseconds(unsigned long us) override;
 unsigned long millis() override;
 unsigned long micros() override;
 long pulseIn(uint32_t pin, uint32_t state, unsigned long timeout) override;

 void spiBegin() override;
 void spiBeginTransaction() override;
 void spiTransfer(uint8_t *out, size_t len, uint8_t *in) override;
 void spiEndTransaction() override;
 void spiEnd() override;

private:
 SPI_HandleTypeDef *hspi;
};

#endif //STM32H562_STMHAL_H
