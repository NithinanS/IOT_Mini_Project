import blynklib
import config
import requests
import serial
import json
import time
import gpiozero as zero

ser = serial.Serial('/dev/ttyAMA0', 9600, timeout=1)

#blynk = blynklib.Blynk(config.BLYNK_AUTH_TOKEN)

TANK_HEIGHT = 15
temp = 0
volume = 0
voltage = 0
tds = 0
open = False

#TODO : change number to the corresponding pin
sensorMotionPIN = 17
sensorServoPIN = 18

servo = zero.Servo(sensorServoPIN, min_pulse_width=0.5/1000, max_pulse_width=2.5/1000)
motion = zero.DigitalInputDevice(sensorMotionPIN)


# @blynk.handle_event("read V0")
# def v0_handler(value) :
#     print(value)


def listenESP() :
    try :
        line = ser.readline().decode('utf-8').strip()
        if line:
            try:
                data = json.loads(line)
                
            except Exception as e:
                print("Error parsing:", line, e)

            else : 
                global temp
                global volume
                global voltage
                global tds
                temp = data['temperature']
                volume = data['waterLevel']
                voltage = data["voltage"]
                tds = data["tdsValue"]
    except : pass

def openTap() :
    # servo.min()
    # print("Min")
    # time.sleep(1)
    # servo.mid()
    # print("Mid")
    # time.sleep(1)
    # servo.max()
    # print("Max")
    # time.sleep(1)
    pass


while True:

    #blynk.run()
    listenESP()
    if (motion.value == 1):
        print("Detected!!!")
        openTap()
    else:
        print("Not detected...")
    print("temp :", temp)
    print("water left :", volume, "%")
    print("conducting value :", voltage)
    print("TDS :", tds)
    time.sleep(0.5)