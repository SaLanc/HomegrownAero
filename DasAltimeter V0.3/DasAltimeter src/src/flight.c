#include <util.h>
#include <boardDefines.h>

#define TESTFLIGHT 0


void flight() {

    switch(flightState) {
    case flightStatrup:

        updateGround( );

        if ((millis() - startupTick) > 10000) {
            findNewFlightStart();
            startupJingle();

#if TESTFLIGHT
            flightState = flightTest;
            writeFlightStartAddress();
            logFlight( );
            logEvent('L');
#else
            flightState = flightPad;
#endif

            break;
        }

        attemptConnection();
        break;

    case flightIdle:

        if (millis() - offsets.groundBeep > 2000) {
            offsets.groundBeep = millis();
            beep(400);
        }

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
        *  Accel > 2g & vel > 15ft/s
        *		 or
        *  baro alt > 100ft
        */

        updateGround();

        if (writeLog) {
            logSensorsOnPad( );
            pinToggle(LedPin);
        }


        //    if ((( velocity > 15) && (accel > 2)) | (altitudeAGL() > 100)) {
        if ((( velocity > 5) && (accel > 2)) | (altitudeAGL() > 100)) {

            flightState = flightBoost;
            beginFlightLog();
        }

        break;
    case flightBoost:
        /*
        *	boost to fast
        *		while
        *	Accel > 1/4G
        */
        if (writeLog) {
            logSensors( );
        }

        if (velocity < 0) {
            flightState = flightDrogue;
            igniteDrogue();
            logEvent('A');
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

        if (writeLog) {
            logSensors( );
        }


//         if (sample.pressureAltitude < deploymentSettings.MAIN_DEPLOY) {
//             flightState = flightMain;
//             igniteMain();
//             logEvent('M');
//         }

        break;
    case flightMain:

        if (writeLog) {
            logSensors( );
        }

        if (velocity < 5) {
            flightState = flightLanded;
            finishFlight();
        }

        break;
    case flightLanded:

        if (millis() - offsets.groundBeep > 2000) {
            offsets.groundBeep = millis();
            beep(400);
        }

        break;
    case flightTest:

        if (writeLog) {
            logSensors( );
        }

        break;
    }



    if ((flightState > flightPad) & unplugged()) {
        finishFlight();
    }

    if ((flightState <= flightPad) & unplugged()) {
        finishFromPad();
    }

}