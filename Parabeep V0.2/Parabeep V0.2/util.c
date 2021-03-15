#include <util.h>

void init() {

    init_clock_source_osculp32k();
    delayInit();
    rtcInit();
    TC2Init();
    TC0Init();

    SPI0init(0);
    SPI5init(5);

    USART3init();
    USART2init();



    pinOut(BuzzerPin);
    pinOut(RstLck);

    pinOut(PeriphEN);
    enablePerif();

    pinOut(LedPin);

    pinOut(spi0MOSI);
    pinMux(spi0MOSI);
    pinOut(spi0SCK);
    pinMux(spi0SCK);
    pinOut(cs_accel);
    pinHigh(cs_accel);
    pinOut(cs_baro);
    pinHigh(cs_baro);
    pinIn(spi0MISO);
    pinMux(spi0MISO);

    pinOut(spi5MOSI);
    pinMux(spi5MOSI);
    pinOut(spi5SCK);
    pinMux(spi5SCK);
    pinOut(cs_mem);
    pinHigh(cs_mem);
    pinIn(spi5MISO);
    pinMux(spi5MISO);

    pinOut(USART2TX);
    pinMux(USART2TX);
    pinIn(USART2RX);
    pinMux(USART2RX);

    pinOut(USART3TX);
    pinMux(USART3TX);
    pinIn(USART3RX);
    pinMux(USART3RX);
// 	pinOut(GPSInt);

    pinOut(RN4870_NRST);
    pinOut(RN4870_MODE);
    pinHigh(RN4870_MODE);
    pinLow(RN4870_NRST);



    initMS5803Barometer();

    adxlWriteByte(ADXL345_REG_POWER_CTL, 0x00);
    adxlWriteByte(ADXL345_REG_DATA_FORMAT, 0x00);
    adxlWriteByte(ADXL345_REG_FIFO_CTL, 0x00);
    adxlWriteByte(ADXL345_REG_POWER_CTL, 0x08);

}

void POST() {

    bool postFailed = false;

    //Baro post first
    u8_t crc = MS5803_CRC4();
    if (crc != coefficients_[7]) {
        postFailed = true;
    }

    //Memory Post
    pinLow(cs_mem);
    spiDataOut(SPI5,0x9f); // read id and mfg code
    volatile u8_t mfgID = spiDataIn(SPI5);
    volatile u8_t deviceID1 = spiDataIn(SPI5);
    volatile u8_t deviceID2 = spiDataIn(SPI5);
    pinHigh(cs_mem);

    if ( (mfgID != 0x1f) & (deviceID1 != 0x86) & (deviceID2 != 0x01)) {
        postFailed = true;
    }

    if (!postFailed) {
        TC2_enable_interupt();
        delay_ms(1000);
        TC2_disable_interupt();
    } else {
        TC2_enable_interupt();
        delay_ms(1000);
        TC2_disable_interupt();
        delay_ms(1000);
        TC2_enable_interupt();
        delay_ms(1000);
        TC2_disable_interupt();
    }


    //start sampling
    TC0_enable_interupt(); //sample

    //TC2_enable_interupt();//buzzer

}

