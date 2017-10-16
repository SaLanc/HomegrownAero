/*
 * DasAltimeter.c
 *
 * Created: 10/4/2017 5:28:21 PM
 * Author : ancar
 */


#include "sam.h"
#include <util.h>
#include <flight.h>
#include <boardDefines.h>




void init() {
    SystemInit();
    GclkInit();
    delayInit();
    adcInit();
    dmaInit();

    NVIC_EnableIRQ(DMAC_IRQn);
    NVIC_SetPriority(DMAC_IRQn, 0x00);

    pinOut(LedPin);
    pinAnalog(senseBat);

    pinOut(spi1MOSI);
    pinOut(spi1SCK);
    pinIn(spi1MISO);
    pinMux(spi1MISO);
    pinMux(spi1SCK);
    pinMux(spi1MOSI);

    //  pinOut(spi2MOSI);
//   pinOut(spi2SCK);
    //  pinIn(spi2MISO);
    //  pinMux(spi2MISO);
    //  pinMux(spi2SCK);
    //  pinMux(spi2MOSI);


    pinOut(cs_imu);
    pinHigh(cs_imu);
    pinGpio(cs_imu);

    pinOut(cs_baro);
    pinHigh(cs_baro);
    pinGpio(cs_baro);


    sercomClockEnable(SPI1, 3, 4);
    sercomSpiMasterInit(SPI1, 3, 0, 0, 0, 0x00);

}


int main(void) {

    init();

    Altimeter my_altimeter;

    initMS5803Barometer(&my_altimeter.myBarometer);

    volatile float batV = 0;

    /* Replace with your application code */

    volatile uint8_t rData;
    volatile uint8_t rData2;
    volatile uint8_t rData3;
    volatile uint8_t rData4;
    volatile long counter = 0;

    volatile int64_t sumAltitude;
    volatile int64_t averageAlt;

    while (1) {

        counter++;
        pinToggle(LedPin);
        batV = (0.0020676 * adc_read(senseBat));

        sampleTick(&my_altimeter);

        uint8_t dummy_Tx = 0xFF;
        uint8_t dummy_rx;

        pinLow(cs_imu);
		dummy_rx = spiDataTransfer(SPI1,ACCEL_CONFIG | 0x80);
		rData = spiDataTransfer(SPI1,dummy_Tx);
		rData2 = spiDataTransfer(SPI1,dummy_Tx);
      pinHigh(cs_imu);



    }
}


