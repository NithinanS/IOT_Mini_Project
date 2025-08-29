import gpiod
import blynklib
import config

blynk = blynklib.Blynk(config.BLYNK_AUTH_TOKEN)

LED_PIN = 12
sensor1_PIN = 17
sensor2_PIN = 18
RxPin = 15
TxPin = 14
chip = gpiod.Chip('gpiochip4')

led_line = chip.get_line(LED_PIN)
sensor1 = chip.get_line(sensor1_PIN)
sensor2 = chip.get_line(sensor2_PIN)
led_line.request(consumer="LED", type=gpiod.LINE_REQ_DIR_OUT)
sensor1.request(consumer="Sensor", type=gpiod.LINE_REQ_DIR_IN)
sensor2.request(consumer="Sensor", type=gpiod.LINE_REQ_DIR_IN)

@blynk.on("connected")
def blynk_connected() :
    print("Blynk is connected to raspberry pi")

@blynk.on("V0")
def v0_handler(value) :
    print(value)

while True :
    blynk.run()