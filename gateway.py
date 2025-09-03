import requests
import serial
import json
import time
import gpiozero as zero
import aiohttp
import asyncio

ser = serial.Serial('/dev/ttyAMA0', 9600, timeout=1)

#blynk = blynklib.Blynk(config.BLYNK_AUTH_TOKEN)

temp = 0
volume = 0
voltage = 0
tds = 0
open = False
reported_temp = 0
reported_volume = 0
reported_voltage = 0
reported_tds = 0
opened = False

sensorMotionPIN = 17
sensorServoPIN = 27

servo = zero.Servo(sensorServoPIN, min_pulse_width=0.5/1000, max_pulse_width=2.5/1000)
motion = zero.DigitalInputDevice(sensorMotionPIN)


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
    servo.mid()
    open = True

def closeTap() :
    global open
    servo.max()
    open = False

async def updateBlynk():
    global reported_volume, reported_voltage, reported_tds, reported_temp, opened

    async with aiohttp.ClientSession() as session:
        tasks = []
        if round(volume) != round(reported_volume):
            tasks.append(session.get(f"https://sgp1.blynk.cloud/external/api/update?token=jRqpqDZlUCBdlLsynkT_ENtKSx38b4bA&v0={volume}"))
            reported_volume = volume
        if open != opened:
            tasks.append(session.get(f"https://sgp1.blynk.cloud/external/api/update?token=jRqpqDZlUCBdlLsynkT_ENtKSx38b4bA&d0={open}"))
            opened = open
        if round(tds) != round(reported_tds):
            tasks.append(session.get(f"https://sgp1.blynk.cloud/external/api/update?token=jRqpqDZlUCBdlLsynkT_ENtKSx38b4bA&v1={tds}"))
            reported_tds = tds
        if round(temp,1) != round(reported_temp,1):
            tasks.append(session.get(f"https://sgp1.blynk.cloud/external/api/update?token=jRqpqDZlUCBdlLsynkT_ENtKSx38b4bA&v2={temp}"))
            reported_temp = temp
        if tasks:
            await asyncio.gather(*tasks)

# max = ล่าง
# min = บน
servo.max() # set default servo postion to lower

last_update = 0

while True:

    #blynk.run()
    listenESP()
    if (motion.value == 1):
        print("Detected!!!")
        openTap()
    else:
        print("Not detected...")
        closeTap()
    print("temp :", temp)
    print("water left :", volume, "%")
    print("conducting value :", voltage)
    print("TDS :", tds)
    now = int(time.time())
    if now % 5 == 0 and now != last_update:
        asyncio.run(updateBlynk())
        last_update = now
    time.sleep(0.5)
