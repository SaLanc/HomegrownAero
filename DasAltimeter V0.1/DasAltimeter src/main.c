/*
 * DasAltimeter.c
 *
 * Created: 10/4/2017 5:28:21 PM
 * Author : ancar
 */


#include "sam.h"
#include <util.h>
#include <boardDefines.h>
#include "math.h"




void init() {
    SystemInit();
    GclkInit();
    RtcInit();
    delayInit();
    adcInit();
    dmaInit();

    NVIC_EnableIRQ(DMAC_IRQn);
    NVIC_SetPriority(DMAC_IRQn, 0x00);

    pinOut(LedPin);
    pinAnalog(senseBatPin);

    pinAnalog(senseAPin);
    pinMux(senseAPin);
    pinOut(fireAPin);
    pinLow(fireAPin);

    pinAnalog(senseBPin);
    pinMux(senseBPin);
    pinOut(fireBPin);
    pinLow(fireBPin);

    pinAnalog(senseCPin);
    pinOut(fireCPin);
    pinLow(fireCPin);

    pinAnalog(senseDPin);
    pinOut(fireDPin);
    pinLow(fireDPin);

    pinOut(spi0MOSI);
    pinOut(spi0SCK);
    pinIn(spi0MISO);
    pinMux(spi0MISO);
    pinMux(spi0SCK);
    pinMux(spi0MOSI);

    pinOut(cs_accel);
    pinHigh(cs_accel);

    pinOut(cs_gyro);
    pinHigh(cs_gyro);

    pinOut(spi1MOSI);
    pinOut(spi1SCK);
    pinIn(spi1MISO);
    pinMux(spi1MISO);
    pinMux(spi1SCK);
    pinMux(spi1MOSI);

    pinOut(cs_mem);
    pinHigh(cs_mem);



    pinOut(spi2MOSI);
    pinOut(spi2SCK);
    pinIn(spi2MISO);
    //  pinMux(spi2MISO);
    //  pinMux(spi2SCK);
    //  pinMux(spi2MOSI);

    pinOut(cs_baro);
    pinHigh(cs_baro);



    pinOut(buzzerPin);
    pinCfg(buzzerPin);

    pinOut(TxPo);
    pinMux(TxPo);
    pinIn(RxPo);
    pinMux(RxPo);

//    sercomClockEnable(SPI2, 3, 4);
//   sercomSpiMasterInit(SPI2, 3, 0, 0, 0, 0x00);

    sercomClockEnable(SPI0, 3, 4);
    sercomSpiMasterInit(SPI0, 3, 0, 0, 0, 0x00);

    sercomClockEnable(SPI1, 3, 4);
    sercomSpiMasterInit(SPI1, 3, 0, 0, 0, 0x00);

    sercomClockEnable(USART3, 4, 8);
    sercomUartInit(USART3,1,0,63858);

    TC4Init();
    TC5Init();
}

void startUp(Altimeter *my_altimeter) {


    beep(400);

    u32_t startupTime = millis();

    sampleTick(my_altimeter);
    flight(my_altimeter);

    while(!my_altimeter->myFlashMemory.pageReady) {
		delay_ms(10);
        sampleTick(my_altimeter);
        logSensors(my_altimeter);
    }

    my_altimeter->myFlashMemory.pageReady = false;


    while((millis() - startupTime) < 5000) {


        if (sercom(USART3)->SPI.INTFLAG.bit.RXC == 1) {
            u8_t data1 = usartDataIn(USART3);
            if (data1 == 0x41) {

                u16_t length = sizeof(my_altimeter->myFlashMemory.pageToWrite);

                usartDataOut(USART3, 'T');

                usartDataOut(USART3, length >> 0);
                usartDataOut(USART3, length >> 8);


                //          AT25SEreadSample(my_altimeter->myFlashMemory.currentAddress, bytesToSend, data);



                for (u8_t dataByte = 0; dataByte < length; ++dataByte) {
                    usartDataOut(USART3, my_altimeter->myFlashMemory.pageToWrite[dataByte]);
                }
                break;
            }
        }
    }

    sampleTick(my_altimeter);
    flight(my_altimeter);

    beep(300);
    delay_ms(80);
    beep(300);
    delay_ms(250);
    beep(300);
    delay_ms(80);
    beep(300);
    delay_ms(500);


}



int main(void) {

    init();

    Altimeter my_altimeter;
    my_altimeter.myFlightState = flightStatrup;
    initMS5803Barometer(&my_altimeter.myBarometer);
    IMUinit();

    sampleTick(&my_altimeter);

    my_altimeter.myFlashMemory.pageLocation = 0x00;


    startUp(&my_altimeter);


    u32_t time = 0;
    u32_t lastTime = 0;
    my_altimeter.myFlashMemory.currentAddress = 0x00000100;
    my_altimeter.myFlashMemory.pageLocation = 0x00;

    AT25SFErace4KBlock(my_altimeter.myFlashMemory.currentAddress);
    //logSensors(&my_altimeter);
    delay_ms(5);



    while (1) {
        time = millis();



        if (takeSample) {
            sampleTick(&my_altimeter);
            flight(&my_altimeter);
            takeSample = false;

        }


        if (writeLog) {
            writeLog = false;

            if (my_altimeter.myFlashMemory.pageReady) {
                my_altimeter.myFlashMemory.pageReady = false;

                pinToggle(LedPin);
                delay_ms(80);
                pinToggle(LedPin);

                //beep(400);



                u16_t logLen = 510;
                usartDataOut(USART3, 'T');
                usartDataOut(USART3, logLen >> 0);
                usartDataOut(USART3, logLen >> 8);

                for (u8_t dataByte = 0; dataByte < 0xFF; ++dataByte) {
                    usartDataOut(USART3, my_altimeter.myFlashMemory.pageToWrite[dataByte]);
                }
                for (u8_t dataByte = 0; dataByte < 0xFF; ++dataByte) {
                    usartDataOut(USART3, my_altimeter.myFlashMemory.pageToWrite[dataByte]);
                }


            }

            logSensors(&my_altimeter);

        }


    }
}


