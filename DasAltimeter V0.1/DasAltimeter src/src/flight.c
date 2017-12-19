#include <util.h>
#include <boardDefines.h>



void flight(Altimeter *my_altimeter) {



    switch(my_altimeter->myFlightState) {
    case flightStatrup:

        my_altimeter->myIMU.gravityOffsetBuffer = 	my_altimeter->myIMU.gravityOffsetBuffer * 0.8 + my_altimeter->myIMU.accelZ * 0.2;
        my_altimeter->myBarometer.groundOffsetBuffer = my_altimeter->myBarometer.groundOffsetBuffer * 0.5 + my_altimeter->Altitude * 0.5;
        my_altimeter->myBarometer.groundTemperatureBuffer = my_altimeter->myBarometer.groundTemperatureBuffer * 0.8 + my_altimeter->myBarometer.temperatureCelcus * 0.2;

        if ((millis() - my_altimeter->myIMU.offsetBufferTime) > 2000) {
            //recursive filter
            my_altimeter->myBarometer.groundOffset = my_altimeter->myBarometer.groundOffsetBuffer;
            my_altimeter->myBarometer.groundTemperature = my_altimeter->myBarometer.groundTemperatureBuffer;
            my_altimeter->myIMU.gravityOffset = my_altimeter->myIMU.gravityOffsetBuffer;
            my_altimeter->myIMU.offsetBufferTime = millis();
        }

        //logSensors(my_altimeter);


        if ((millis() - my_altimeter->StartupTick) > 10000) {
            findNewFlightStart(my_altimeter);
            logFlight(my_altimeter);
            startupJingle();
			my_altimeter->myFlightState = flightTest;
            //my_altimeter->myFlightState = flightPad;
            break;
        }

        attemptConnection(my_altimeter);



        if (my_altimeter->myVoltages.batFloat < 3.5) {
            my_altimeter->myFlightState = flightIdle;
            delay_ms(80);
            beep(300);
            delay_ms(80);
            beep(300);
            delay_ms(80);
            beep(300);
            delay_ms(80);
            pinLow(buzzerPin);
            pinLow(LedPin);
        }

        break;
    case flightIdle:

        //TC4->COUNT8.CTRLA.reg = 0;
        //TC5->COUNT8.CTRLA.reg = 0;

        //delay_ms(1000);
        //pinToggle(LedPin);

        if (sercom(USART3)->SPI.INTFLAG.bit.RXC == 1) {
            u8_t possibleReset = usartDataIn(USART3);
            if (possibleReset == 0x52) {
                NVIC_SystemReset();
            }
        }

        break;
    case flightPad:
        /* from pad to boost
        *  initial trigger attempt will be
        *  Accel > 2g & vel > 5m/s
        *		 or
        *  baro alt > 40ft
        */

        my_altimeter->myIMU.gravityOffsetBuffer = 	my_altimeter->myIMU.gravityOffsetBuffer * 0.8 + my_altimeter->myIMU.accelZ * 0.2;
        my_altimeter->myBarometer.groundOffsetBuffer = my_altimeter->myBarometer.groundOffsetBuffer * 0.5 + my_altimeter->Altitude * 0.5;
        my_altimeter->myBarometer.groundTemperatureBuffer = my_altimeter->myBarometer.groundTemperatureBuffer * 0.8 + my_altimeter->myBarometer.temperatureCelcus * 0.2;

        if ((millis() - my_altimeter->myIMU.offsetBufferTime) > 2000) {
            //recursive filter
            my_altimeter->myBarometer.groundOffset = my_altimeter->myBarometer.groundOffsetBuffer;
            my_altimeter->myBarometer.groundTemperature = my_altimeter->myBarometer.groundTemperatureBuffer;
            my_altimeter->myIMU.gravityOffset = my_altimeter->myIMU.gravityOffsetBuffer;
            my_altimeter->myIMU.offsetBufferTime = millis();
            break;
        }

        if (writeLog) {
            writeLog = false;
            //logSensors(my_altimeter);
            pinToggle(LedPin);
        }

        if ((my_altimeter->Velocity > 0.15) && ((my_altimeter->Altitude - my_altimeter->myBarometer.groundOffset) > 6)) {
            my_altimeter->myFlightState = flightBoost;
            logEvent(my_altimeter, 'L');
        }



        if (my_altimeter->myVoltages.batFloat < 3.5) {
            my_altimeter->myFlightState = flightIdle;
            AT25SFHoldTillReady();
            writeFlightEndAddress(my_altimeter);
            delay_ms(80);
            beep(300);
            delay_ms(80);
            beep(300);
            delay_ms(80);
            beep(300);
            delay_ms(80);
            pinLow(buzzerPin);
            pinLow(LedPin);
        }

        break;
    case flightBoost:
        /*
        *	boost to fast
        *		while
        *	Accel > 1/4G
        */

        if (writeLog) {
            writeLog = false;
            logSensors(my_altimeter);
            if (my_altimeter->myFlashMemory.pageReady) {
                my_altimeter->myFlashMemory.pageReady = false;
                pinToggle(LedPin);
                u8_t bytesWritten = AT25SEWritePage(my_altimeter->myFlashMemory.currentAddress,my_altimeter->myFlashMemory.pageToWrite);
                my_altimeter->myFlashMemory.currentAddress = (my_altimeter->myFlashMemory.currentAddress + 0x100);
            }
        }


        if (my_altimeter->Velocity < 0) {
            my_altimeter->myFlightState = flightDrogue;
            logEvent(my_altimeter, 'A');
            beep(100);

        }

        if (my_altimeter->myVoltages.batFloat < 3.5) {
            my_altimeter->myFlightState = flightIdle;
            AT25SFHoldTillReady();
            writeFlightEndAddress(my_altimeter);
            delay_ms(80);
            beep(300);
            delay_ms(80);
            beep(300);
            delay_ms(80);
            beep(300);
            delay_ms(80);
            pinLow(buzzerPin);
            pinLow(LedPin);
        }

        break;
    case flightFast:




        break;
    case flightCoast:

        /* Coast to drogue
        *		while
        *	   speed > 0
        *	   	 &
        *	altitude > baro_max_alt
        */


        break;
    case flightDrogue:


        if (my_altimeter->myFlashMemory.pageReady) {
            my_altimeter->myFlashMemory.pageReady = false;
            pinToggle(LedPin);
            //AT25SFHoldTillReady();
            u8_t bytesWritten = AT25SEWritePage(my_altimeter->myFlashMemory.currentAddress,my_altimeter->myFlashMemory.pageToWrite);
            my_altimeter->myFlashMemory.currentAddress = (my_altimeter->myFlashMemory.currentAddress + 0x100);
        }


        if (my_altimeter->myVoltages.batFloat < 3.5) {
            my_altimeter->myFlightState = flightIdle;
            AT25SFHoldTillReady();
            writeFlightEndAddress(my_altimeter);
            delay_ms(80);
            beep(300);
            delay_ms(80);
            beep(300);
            delay_ms(80);
            beep(300);
            delay_ms(80);
            pinLow(buzzerPin);
            pinLow(LedPin);
        }
        break;
    case flightMain:



        break;
    case flightLanded:



        break;
    case flightTest:

        if (writeLog) {
            writeLog = false;
            logSensors(my_altimeter);
            if (my_altimeter->myFlashMemory.pageReady) {
                my_altimeter->myFlashMemory.pageReady = false;
                pinToggle(LedPin);
                u8_t bytesWritten = AT25SEWritePage(my_altimeter->myFlashMemory.currentAddress,my_altimeter->myFlashMemory.pageToWrite);
                my_altimeter->myFlashMemory.currentAddress = (my_altimeter->myFlashMemory.currentAddress + 0x100);
            }
        }
        if (my_altimeter->myVoltages.batFloat < 3.5) {
            my_altimeter->myFlightState = flightIdle;
            AT25SFHoldTillReady();
            writeFlightEndAddress(my_altimeter);
            delay_ms(80);
            beep(300);
            delay_ms(80);
            beep(300);
            delay_ms(80);
            beep(300);
            delay_ms(80);
            pinLow(buzzerPin);
            pinLow(LedPin);
        }


        break;
    }


}
