#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <WiFiClient.h>
// #include <SoftwareSerial.h>
#include <ESP32Servo.h>

#define ULTRA_SONIC_TRIG 18 
#define ULTRA_SONIC_ECHO 17 

#define SERVO_PIN 19

Servo myServo;

// Temperature Measure
// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
// --------------------------------------------

/* -------------------------------- */
 /*เมื่อน้ำถึงระดับเซนเซอร์ จะมีเสียงบัซเซอร์ดังขึ้น */
#define WATER_SENSOR 2 // ขา SIG ต่อกับขา D3 ของ Arduino
#define BUZZER 3// บัซเซอร์ต่อกับขา D2 ของ Arduino
/* -------------------------------- */

/* RT TX  */
#define TxPin 22
#define RxPin 23
// SoftwareSerial anotherSerial(RxPin, TxPin);

void pins_init() {
  pinMode(WATER_SENSOR, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(ULTRA_SONIC_TRIG, OUTPUT);
  pinMode(ULTRA_SONIC_ECHO, INPUT);

  pinMode(SERVO_PIN, OUTPUT);
	myServo.attach(SERVO_PIN);
  myServo.write(90);


}

void connectWifi() {

  char ssid[] = "404"; // Dami 14
  char password[] = "N0tFr33Wifi"; // BigMi1414

  /* Connecting to WIFI */
	Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password); // เริ่มต้นเชื่อมต่อ WiFi

  while (WiFi.status() != WL_CONNECTED) { // วนลูปหากยังเชื่อมต่อ WiFi ไม่สำเร็จ
      delay(500);
      Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

float readWaterTemperature() {
    sensors.requestTemperatures(); // Send the command to get temperatures
    return sensors.getTempCByIndex(0); // Read temperature in Celsius from first sensor
}

void mesureWaterLevel() {
	// Ultra Sonic Sensor
	digitalWrite(ULTRA_SONIC_TRIG, HIGH);
	delayMicroseconds(10);
	digitalWrite(ULTRA_SONIC_TRIG, LOW);

	// measure duration of pulse from ECHO pin
	float duration_us = pulseIn(ULTRA_SONIC_ECHO, HIGH);
	float distance_cm = 0.017 * duration_us;

	Serial.println("Distance:" +  String(distance_cm) + " cm");

	// if(distance_cm <= 70) {
	// 	if (!haveNotified) {
	// 		LINE.send("Visitor Detected");
	// 		haveNotified = true;
	// 		anotherSerial.println("d=1"); // Send to gateway board
	// 	}
	// 	Serial.println("life detected");
		
	// }
	// else {
	// 	Serial.println("zzz...");
	// 	haveNotified = false;
	// 	anotherSerial.println("d=0"); // Send to gateway board

	// }
}

/************************************************************************/
/* ฟังก์ชันสร้างเสียงร้องบัซเซอร์ เป็นเวลา 2 วินาที */
void soundAlarm() {
    for(uint8_t i = 0;i < 20;i ++)
    {
        digitalWrite(BUZZER, HIGH);
        delay(50);
        digitalWrite(BUZZER, LOW);
        delay(50);
    }
}
/************************************************************************/
/*ฟังก์ชัน เช็คระดับน้ำจากตัวเซนเซอร์ ถ้าถึงระดับเซนเซอร์ จะให้ค่า true กลับไป ถ้าไม่ถึงส่งค่า false       */

boolean isExposedToWater() {
    if(digitalRead(WATER_SENSOR) == LOW)
        return true;
    else return false;
}

/************************************************************************/

void openDoor() {
	Serial.println("Door automatically opened.");
	myServo.write(180);  // เปิดประตู
}

void closeDoor() {
	Serial.println("Door automatically closed.");
	myServo.write(0);  // ปิดประตู
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  // sensors.begin();
  pins_init();
  closeDoor();
  
}

void loop() {
  mesureWaterLevel();

  // Example of opening and closing the door every 5 seconds
  Serial.println("Opening door...");
  openDoor();
  delay(5000);

  Serial.println("Closing door...");
  closeDoor();
  delay(5000);
}
