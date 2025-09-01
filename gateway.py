import gpiod
import blynklib
import config
import requests
import serial
import json
import RPi.GPIO as GPIO
from gpiozero import servo

ser = serial.Serial('/dev/ttyAMA0', 9600, timeout=1)
OBSTACLE_PIN = 'D0'

blynk = blynklib.Blynk(config.BLYNK_AUTH_TOKEN)

TANK_HEIGHT = 15
temp = 0
volume = 0
open = False

#TODO : change number to the corresponding pin
LED_PIN = 12
sensor1_PIN = 17
sensor2_PIN = 18
chip = gpiod.Chip('gpiochip4')

led_line = chip.get_line(LED_PIN)
motion = chip.get_line(sensor1_PIN)
servo = chip.get_line(sensor2_PIN)
led_line.request(consumer="LED", type=gpiod.LINE_REQ_DIR_OUT)
motion.request(consumer="Motion", type=gpiod.LINE_REQ_DIR_IN)
servo.request(consumer="Motor", type=gpiod.LINE_REQ_DIR_OUT)

@blynk.on("connected")
def blynk_connected() :
    print("Blynk is connected to raspberry pi")

@blynk.on("V0")
def v0_handler(value) :
    print(value)

def startSensorMotion() :
    sensorValue = motion.get_value()
    return (sensorValue == 0)

def listenESP() :
    line = ser.readline().decode('utf-8').strip()
    if line:
        try:
            data = json.loads(line)
            print(f"Temperature: {data['temperature']}°C, Distance: {data['distance']} cm")
            
        except Exception as e:
            print("Error parsing:", line, e)

        else : 
            temp = data['temperature']
            volume = min(round((TANK_HEIGHT - data['distance'])*100/TANK_HEIGHT, 2), 100)

def openTap() :
    pass

try :
    while True :
        blynk.run()
        startSensorMotion()
        listenESP()
finally :
    led_line.release()
    motion.release()
    servo.release()
