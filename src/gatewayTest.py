import blynklib
import config
import requests
import serial
import json
import time
import gpiozero as zero

ser = serial.Serial('/dev/ttyAMA0', 9600, timeout=1)

#blynk = blynklib.Blynk(config.BLYNK_AUTH_TOKEN)

temp = 0
volume = 0
voltage = 0
tds = 0
open = False

sensorMotionPIN = 17
sensorServoPIN = 27

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
    global open
    servo.min()
    open = True

def closeTap() :
    global open
    servo.max()
    open = False

def toggleTap(status) :
    global open
    if status and not open:
        openTap()
    elif not status and open:
        closeTap()

def updateBlynk() :
    #water left
    requests.get(f"https://sgp1.blynk.cloud/external/api/update?token=jRqpqDZlUCBdlLsynkT_ENtKSx38b4bA&v0={volume}")
    #water dispensing
    requests.get(f"https://sgp1.blynk.cloud/external/api/update?token=jRqpqDZlUCBdlLsynkT_ENtKSx38b4bA&d0={open}")
    #water quality
    requests.get(f"https://sgp1.blynk.cloud/external/api/update?token=jRqpqDZlUCBdlLsynkT_ENtKSx38b4bA&v1={tds}")
    #water temp
    requests.get(f"https://sgp1.blynk.cloud/external/api/update?token=jRqpqDZlUCBdlLsynkT_ENtKSx38b4bA&v2={temp}")



# max = ล่าง
# min = บน
servo.max()

while True:

    # blynk.run()
    listenESP()
    if (motion.value == 1):
        print("Detected!!!")
        toggleTap(True)
    else:
        print("Not detected...")
        toggleTap(False)
    print("temp :", temp)
    print("water left :", volume, "%")
    print("conducting value :", voltage)
    print("TDS :", tds)
    if(round(time.time() % 10) == 0) :
        updateBlynk()
    time.sleep(0.025)