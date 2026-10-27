#include "stmHal.h"

// --- pin table: index = the pin IDs you pass to Module / setRfSwitchPins ---
struct PinDef
{
 GPIO_TypeDef* port;
 uint16_t pin;
};

static const PinDef pins[] = {
 {LORA_CS_GPIO_Port, LORA_CS_Pin},
 {LORA_RST_GPIO_Port, LORA_RST_Pin},
 {LORA_BUSY_GPIO_Port, LORA_BUSY_Pin},
 {LORA_DIO1_GPIO_Port, LORA_DIO1_Pin},
 {LORA_RXEN_GPIO_Port, LORA_RXEN_Pin},
 {LORA_TXEN_GPIO_Port, LORA_TXEN_Pin},
};

// callbacks, indexed by EXTI line number (0..15)
static void (*irqCb[16])(void);

// --- constructor: passes Arduino-style constants to the base class ---
StmHal::StmHal(SPI_HandleTypeDef* spi)
 : RadioLibHal(0, 1, 0, 1, 1, 2), // input, output, low, high, rising, falling
   hspi(spi)
{
}

void StmHal::init()
{
 DCB->DEMCR |= DCB_DEMCR_TRCENA_Msk; // cycle counter for micros()
 DWT->CYCCNT = 0;
 DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

void StmHal::term()
{
}

// pin modes are set in CubeMX, so nothing to do
void StmHal::pinMode(uint32_t, uint32_t)
{
}

void StmHal::digitalWrite(uint32_t pin, uint32_t value)
{
 if (pin == RADIOLIB_NC) return;
 HAL_GPIO_WritePin(pins[pin].port, pins[pin].pin,
                   value ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

uint32_t StmHal::digitalRead(uint32_t pin)
{
 if (pin == RADIOLIB_NC) return 0;
 return HAL_GPIO_ReadPin(pins[pin].port, pins[pin].pin) == GPIO_PIN_SET;
}

void StmHal::attachInterrupt(uint32_t pin, void (*cb)(), uint32_t)
{
 if (pin == RADIOLIB_NC) return;
 irqCb[__builtin_ctz(pins[pin].pin)] = cb;
}

void StmHal::detachInterrupt(uint32_t pin)
{
 if (pin == RADIOLIB_NC) return;
 irqCb[__builtin_ctz(pins[pin].pin)] = nullptr;
}

void StmHal::delay(unsigned long ms) { HAL_Delay(ms); }

void StmHal::delayMicroseconds(unsigned long us)
{
 uint32_t start = DWT->CYCCNT;
 uint32_t ticks = us * (SystemCoreClock / 1000000UL);
 while ((DWT->CYCCNT - start) < ticks)
 {
 }
}

unsigned long StmHal::millis() { return HAL_GetTick(); }
unsigned long StmHal::micros() { return DWT->CYCCNT / (SystemCoreClock / 1000000UL); }
long StmHal::pulseIn(uint32_t, uint32_t, unsigned long) { return 0; }

void StmHal::spiBegin()
{
}

void StmHal::spiBeginTransaction()
{
}

void StmHal::spiTransfer(uint8_t* out, size_t len, uint8_t* in)
{
 HAL_SPI_TransmitReceive(hspi, out, in, len, 20);
}

void StmHal::spiEndTransaction()
{
}

void StmHal::spiEnd()
{
}

// The ST HAL calls this C function when a rising-edge EXTI fires.
// extern "C" stops C++ from renaming it, so the HAL's call finds it.
extern "C" void HAL_GPIO_EXTI_Rising_Callback(uint16_t pin)
{
 uint8_t n = __builtin_ctz(pin);
 if (irqCb[n]) irqCb[n]();
}

#if 0
#include <RadioLib.h>
#include "main.h"

// Pin IDs RadioLib will pass around; map them to port/pin here
enum { PIN_LORA_CS = 0, PIN_LORA_RST, PIN_LORA_BUSY, PIN_LORA_DIO1 };

struct PinDef
{
 GPIO_TypeDef* port;
 uint16_t pin;
};
static const PinDef pins[] = {
 {LORA_CS_GPIO_Port, LORA_CS_Pin},
 {LORA_RST_GPIO_Port, LORA_RST_Pin},
 {LORA_BUSY_GPIO_Port, LORA_BUSY_Pin},
 {LORA_DIO1_GPIO_Port, LORA_DIO1_Pin},
};

static void (*irqCb[16])(void); // indexed by EXTI line number

class StmHal : public RadioLibHal
{
public:
 StmHal(SPI_HandleTypeDef* spi)
  : RadioLibHal(0, 1, 0, 1, 1, 2), hspi(spi)
 {
 } // input, output, low, high, rising, falling

 void init() override
 {
  // cycle counter for microsecond timing
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CYCCNT = 0;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
 }

 void term() override
 {
 }

 // GPIO (pin modes are configured in CubeMX, so these are mostly no-ops)
 void pinMode(uint32_t, uint32_t) override
 {
 }

 void digitalWrite(uint32_t pin, uint32_t val) override
 {
  if (pin == RADIOLIB_NC) return;
  HAL_GPIO_WritePin(pins[pin].port, pins[pin].pin, val ? GPIO_PIN_SET : GPIO_PIN_RESET);
 }

 uint32_t digitalRead(uint32_t pin) override
 {
  if (pin == RADIOLIB_NC) return 0;
  return HAL_GPIO_ReadPin(pins[pin].port, pins[pin].pin) == GPIO_PIN_SET;
 }

 // Interrupts: EXTI is set up in CubeMX, we just store the callback
 void attachInterrupt(uint32_t pin, void (*cb)(void), uint32_t) override
 {
  if (pin == RADIOLIB_NC) return;
  irqCb[__builtin_ctz(pins[pin].pin)] = cb;
 }

 void detachInterrupt(uint32_t pin) override
 {
  if (pin == RADIOLIB_NC) return;
  irqCb[__builtin_ctz(pins[pin].pin)] = nullptr;
 }

 // Timing
 void delay(unsigned long ms) override { HAL_Delay(ms); }

 void delayMicroseconds(unsigned long us) override
 {
  uint32_t start = DWT->CYCCNT;
  uint32_t ticks = us * (SystemCoreClock / 1000000UL);
  while ((DWT->CYCCNT - start) < ticks)
  {
  }
 }

 unsigned long millis() override { return HAL_GetTick(); }
 unsigned long micros() override { return DWT->CYCCNT / (SystemCoreClock / 1000000UL); }
 long pulseIn(uint32_t, uint32_t, unsigned long) override { return 0; } // unused by most modules

 // SPI
 void spiBegin() override
 {
 }

 void spiBeginTransaction() override
 {
 }

 void spiTransfer(uint8_t* out, size_t len, uint8_t* in) override
 {
  HAL_SPI_TransmitReceive(hspi, out, in, len, 100);
 }

 void spiEndTransaction() override
 {
 }

 void spiEnd() override
 {
 }

private:
 SPI_HandleTypeDef* hspi;
};

// The H5 HAL splits EXTI callbacks by edge
extern "C" void HAL_GPIO_EXTI_Rising_Callback(uint16_t pin)
{
 uint8_t n = __builtin_ctz(pin);
 if (irqCb[n]) irqCb[n]();
}
#endif
