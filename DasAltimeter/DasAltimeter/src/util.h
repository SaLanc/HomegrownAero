#pragma once

#include <samd21g18a.h>
#include <samd21.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



typedef uint8_t u8_t;
typedef uint16_t u16_t;
typedef uint32_t u32_t;

#define SERCOM_HANDLER_(ID) SERCOM ## ID ## _Handler()
#define SERCOM_HANDLER(ID) SERCOM_HANDLER_(ID)

typedef uint8_t DmaChan;
typedef uint8_t SercomId;





// my includes
#include <port.h>
#include <altimeter.h>

//dma.c
inline static Sercom* sercom(SercomId id) {
	return (Sercom*) (0x42000800U + id * 1024);
}

#define DMA_DESC_ALIGN __attribute__((aligned(16)))

void dmaInit();
void dmaSercomStartTx(DmaChan chan, SercomId id, u8_t* src, unsigned size);
void dmaSercomStartRx(DmaChan chan, SercomId id, u8_t* dst, unsigned size);
void dmaAbort(DmaChan chan);
void dmaEnableInterrupt(DmaChan chan);
void dmaFillSercomTx(DmacDescriptor* desc, SercomId id, u8_t *src, unsigned size);
void dmaFillSercomRx(DmacDescriptor* desc, SercomId id, u8_t *dst, unsigned size);
void dmaSercomConfigureTx(DmaChan chan, SercomId id);
void dmaSercomConfigureRx(DmaChan chan, SercomId id);
void dmaLinkChain(DmacDescriptor* chain, u32_t count);
void dmaStartDescriptor(DmaChan chan, DmacDescriptor* chain);
u32_t dmaRemaining(DmaChan chan);

//clocks.c
void GclkInit();
void gclkEnable(u32_t id, u32_t src, u32_t div);
void delayInit(void);
void delay_ms(uint32_t delay);
void delay_us(uint32_t delay);

//sercom.c
void sercomClockEnable(SercomId id, uint32_t clock_channel, u8_t divider);
void sercomReset(SercomId id);
void sercomSpiSlaveInit(SercomId id, u32_t dipo, u32_t dopo, bool cpol, bool cpha);
void sercomSpiMasterInit(SercomId id, u32_t dipo, u32_t dopo, bool cpol, bool cpha, u8_t baud);
void sercomI2cMasterInit(SercomId id, u8_t baud);
void sercomUartInit(SercomId id, u32_t rxpo, u32_t txpo, u32_t baud);


//analog.c
void adcInit();
u16_t adcSample();
u16_t adc_read(Pin p);



//bigBang.c
void byteOut(Pin SCK_PIN, Pin MOSI_PIN, uint8_t byte);
uint8_t byteIn(Pin SCK_PIN, Pin MISO_PIN);


#define CONTAINER_OF(ptr, type, field_name) \
((type *) (((uint8_t *)ptr) - offsetof(type, field_name)))

#define ATOMIC_SECTION_ENTER   { register uint32_t __atomic; \
	__asm volatile ("mrs %0, primask" : "=r" (__atomic) ); \
	__asm volatile ("cpsid i");
#define ATOMIC_SECTION_LEAVE   __asm volatile ("msr primask, %0" : : "r" (__atomic) ); }



static inline void delay_cycles(
const uint32_t n) {
	if (n > 0) {
		SysTick->LOAD = n;
		SysTick->VAL = 0;

		while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)) {
		};
	}
}

static inline void SPI_sync(SercomId id) {
	while  (sercom(id)->SPI.SYNCBUSY.bit.CTRLB);
};