//#include <util.h>

//frostburn
const static Pin cs_mem = {.group = 0, .pin = 20 };
const static Pin spi1MISO = {.group = 0, .pin = 16, .mux = MUX_PA16C_SERCOM1_PAD0 };
const static Pin spi1SCK = {.group = 0, .pin = 17, .mux = MUX_PA17C_SERCOM1_PAD1};
const static Pin spi1MOSI = {.group = 0, .pin = 19, .mux = MUX_PA19C_SERCOM1_PAD3 };

// "salaltimeter
//    const static Pin cs_mem = {.group = 0, .pin = 8 };
//    const static Pin spi1MISO = {.group = 0, .pin = 14 };
//    const static Pin spi1SCK = {.group = 0, .pin = 15};
//    const static Pin spi1MOSI = {.group = 0, .pin = 13 };

const static Pin LedPin = {.group = 0, .pin = 27 };
const static Pin senseBat = {.group = 0, .pin = 6, .chan = 6 };

#define DMA_SERCOM1_TX 1
#define DMA_SERCOM1_RX 2