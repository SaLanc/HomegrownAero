import serial
import twosComp
import sensorPoint
import time
import matplotlib.pyplot as plt
from kalmanFliter import filter_data_with_kalman
import numpy

#from sensorPoint import build_sensor_point
#import sensorPoint

from sensorPoint import SensorPointType


ser = serial.Serial()
ser.baudrate = 19200
ser.port = 'COM14'
ser.timeout = 5





class FlightPoint:
    def __init__(self):
        self.FlightNumb = 0
        self.bufferTick = 0
        self.groundOffset = 0
        self.groundTemperature = 0
        self.groundAccel = 0


pointList = []
flight = FlightPoint()

data = []

sensor_sample = 0
sensor_sample_part = 0

input(' Wait for startup beep and press ENTER')

ser.open()

ser.write(b'H')

handshake = ser.read(1)

if handshake == b'H':
    print('')
    print(' Altimeter connected, what would you like to do?')
    print('     P    Print sensors')
    print('     L    Download flight log')
    print('     E    Chip Erase')
    print('')

    option = input(' ').upper()

    print('')

    if option == 'E':
        ser.write(b'E')
        print('')
        print(' Please wait.')

        time.sleep(25)
        isWriteDone = ser.read(1)
        if isWriteDone == b'E':
            print('')
            print('Chip erased')
            input(' Wait for startup beep and press ENTER')

    if option == 'L':

        ser.write(b'L')

        print(' Which flight log would you like to download?')

        allFlightsFound = 1
        while allFlightsFound:
            flightNumb = ser.read(1)

            if flightNumb == b'\x0F':
                break
            else:
                print('    ', flightNumb.decode('utf-8'))

        print('')
        flightToRead = input(' ')
        print('')
        ser.write(flightToRead.encode('utf-8'))

        StartTime = time.clock()

        pages_to_read = ser.read(2)

        # print(pages_to_read[0] << 0, ' ', pages_to_read[1] << 8)

        pages = (pages_to_read[0] << 0) + (pages_to_read[1] << 8)

        # print(pagesToRead)
        # pages = int.from_bytes(pagesToRead, byteorder='big')
        print('Reading', ((pages * 256) / 1000), 'kilobytes of data.')
        # print('reading',pages)

        flightEndingAddressbytes = ser.read(3)
        flightEndingAddress = (flightEndingAddressbytes[0] << 0) + (flightEndingAddressbytes[1] << 8) + (
        flightEndingAddressbytes[2] << 16)

        memoryPercent = (flightEndingAddress / 1048575) * 100

        # print('Memory is', memoryPercent,'% full.')
        print('Memory is {0:2.2f}% full.'.format(memoryPercent))


    for page in range(0, pages):
        PageOfData = []
        PageOfData = ser.read(256)

        data.append(PageOfData)

    ser.close()

CurrentPage = 0
LocationInPage = 0
ProcessLog = True

samplenum = 0

while ProcessLog:

    if CurrentPage >= pages:
        ProcessLog = False
        break

    # Flight point decode
    if data[CurrentPage][LocationInPage] == 0x46:
        # print('F found, location', LocationInPage)

        if (LocationInPage + 17) >= 255:

            if (CurrentPage + 1) >= pages:
                ProcessLog = False
                break

            sensor_sample_part = data[CurrentPage][LocationInPage:255]
            sensor_sample = sensor_sample_part + data[CurrentPage + 1][0:(16 - (256 - LocationInPage))]

            point = FlightPoint()
            point.FlightNumb = sensor_sample[1]
            point.bufferTick = int.from_bytes(sensor_sample[2:5], byteorder='little')
            point.groundOffset = int.from_bytes(sensor_sample[6:9], byteorder='little')
            point.groundTemperature = int.from_bytes(sensor_sample[10:13], byteorder='little')
            point.groundAccel = twosComp.twos_complement(sensor_sample[14], sensor_sample[15]) * 0.0078125

            CurrentPage += 1
            LocationInPage -= 240  # rollover + 16

        else:
            sensor_sample = data[CurrentPage][LocationInPage:LocationInPage + 16]

            point = FlightPoint()
            point.FlightNumb = sensor_sample[1]
            point.bufferTick = int.from_bytes(sensor_sample[2:5], byteorder='little')
            point.groundOffset = int.from_bytes(sensor_sample[6:9], byteorder='little')
            point.groundTemperature = int.from_bytes(sensor_sample[10:13], byteorder='little')
            point.groundAccel = twosComp.twos_complement(sensor_sample[14], sensor_sample[15]) * 0.0078125

            LocationInPage += 17

        flight = point

    # Sensor point decoding
    if data[CurrentPage][LocationInPage] == 0x41:
        # print('A found, page', CurrentPage, ' ,Location', LocationInPage, ',Sample number:', samplenum)
        samplenum += 1

        if pointList.__len__() > 0:
            lastTick = pointList[pointList.__len__() - 1].sampleTick
        else:
            lastTick = 0

        point, CurrentPage, LocationInPage = sensorPoint.build_sensor_point(data, CurrentPage, pages, LocationInPage,
                                                                            lastTick)
        if isinstance(point, SensorPointType): pointList.append(point)



elapsedTime = (time.clock() - StartTime)
print('Took:', elapsedTime, 'seconds to read and process', pointList.__len__(), 'data points')

print('Flight Numb:', flight.FlightNumb, 'Buffer Time:', flight.bufferTick, 'Ground Offset:', flight.groundOffset,
      'Ground Temperature', flight.groundTemperature)

plt.figure()

ax1 = plt.subplot(2, 1, 1)
ax2 = plt.subplot(2, 1, 2)


x = 0

accelplot = []
heightplot = []
tick = []

previousPoint = sensorPoint.SensorPointType()

for pointToPlot in pointList:

    pointToPlot.sampleTick = pointToPlot.sampleTick - flight.bufferTick

    if x == 0:

        print('Sample 0 tick:', pointToPlot.sampleTick, 'Height Feet:', pointToPlot.heightFeet, 'AccelX:',
              pointToPlot.accelX, 'AccelY:', pointToPlot.accelY, 'AccelZ:', pointToPlot.accelZ)

    elif x > 0:

        dt = pointToPlot.sampleTick - previousPoint.sampleTick
        print('Sample', x, 'tick:', pointToPlot.sampleTick, 'Sample DT:', pointToPlot.Dt, 'Height Feet:', pointToPlot.heightFeet,
            'AccelX:',
             pointToPlot.accelX, 'AccelY:', pointToPlot.accelY, 'AccelZ:', pointToPlot.accelZ)

    tick.append(pointToPlot.sampleTick)
    accelplot.append(pointToPlot.accelZ)
    heightplot.append(pointToPlot.heightFeet)

    #ax1.plot(pointToPlot.sampleTick, pointToPlot.accelZ, 'b.')

    #ax2.plot(pointToPlot.sampleTick, pointToPlot.heightFeet, 'r.')

    previousPoint = pointToPlot
    x += 1

    #stdaccel.append(pointToPlot.accelZ)
    #stdheight.append(pointToPlot.heightFeet)

#print('accel StdDev ', numpy.std(stdaccel))
#print('height StdDev ', numpy.std(stdheight))

ax1.plot(tick,accelplot)
ax2.plot(tick,heightplot)

plt.show()
print('')
input('press ENTER to close')
