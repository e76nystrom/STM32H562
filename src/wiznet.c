#include <stdio.h>

#include "main.h"
#include "wizchip_conf.h"
#include "spi.h"

static void cs_sel(void)
{
 HAL_GPIO_WritePin(W5500_CS_GPIO_Port, W5500_CS_Pin, GPIO_PIN_RESET);
}
static void cs_desel(void)
{
 HAL_GPIO_WritePin(W5500_CS_GPIO_Port, W5500_CS_Pin, GPIO_PIN_SET);
}

static uint8_t spi_rb(void)
{
 uint8_t rx, tx = 0xFF;
 HAL_SPI_TransmitReceive(&hspi2, &tx, &rx, 1, HAL_MAX_DELAY);
 return rx;
}

static void spi_wb(uint8_t b)
{
 HAL_SPI_Transmit(&hspi2, &b, 1, HAL_MAX_DELAY);
}

static void spi_rb_burst(uint8_t* buf, uint16_t len)
{
 HAL_SPI_Receive(&hspi2, buf, len, HAL_MAX_DELAY);
}

static void spi_wb_burst(uint8_t* buf, uint16_t len)
{
 HAL_SPI_Transmit(&hspi2, buf, len, HAL_MAX_DELAY);
}

void w5500_init(void)
{
 // Hardware reset
 HAL_GPIO_WritePin(W5500_RST_GPIO_Port, W5500_RST_Pin, GPIO_PIN_RESET);
 HAL_Delay(2);
 HAL_GPIO_WritePin(W5500_RST_GPIO_Port, W5500_RST_Pin, GPIO_PIN_SET);
 HAL_Delay(100); // W5500 needs ~50 ms+ after reset

 reg_wizchip_cs_cbfunc(cs_sel, cs_desel);
 reg_wizchip_spi_cbfunc(spi_rb, spi_wb);
 reg_wizchip_spiburst_cbfunc(spi_rb_burst, spi_wb_burst);

 uint8_t bufsize[2][8] = {
  {2, 2, 2, 2, 2, 2, 2, 2},
  {2, 2, 2, 2, 2, 2, 2, 2}
 }; // KB per socket

 if (wizchip_init(bufsize[0], bufsize[1]) != 0)
 {
  printf("wizchip init failed\n");
 }

 // Sanity check: should read 0x04 on a W5500
 const uint8_t ver = getVERSIONR();
 printf("wizchip version %d\n", ver);

 wiz_NetInfo net = {
  .mac = {0x02, 0x08, 0xDC, 0x11, 0x22, 0x33},
  .ip = {192, 168, 1, 50},
  .sn = {255, 255, 255, 0},
  .gw = {192, 168, 1, 1},
  .dns = {8, 8, 8, 8},
  .dhcp = NETINFO_STATIC
 };
 wizchip_setnetinfo(&net);

 // Wait for link
 printf("wizchip waiting for link\n");
 while (wizphy_getphylink() == PHY_LINK_OFF)
 {
  HAL_Delay(100);
 }
}
