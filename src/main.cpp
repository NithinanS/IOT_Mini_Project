#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <WiFiClient.h>
// #include <SoftwareSerial.h>
#include <ESP32Servo.h>
#include <ArduinoJson.h>
#include <iomanip>

#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define ULTRA_SONIC_TRIG 18 
#define ULTRA_SONIC_ECHO 17 

#define SERVO_PIN 19

Servo myServo;

// Temperature Measure
// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 5 // Temperature sensors

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
// --------------------------------------------

/* -------------------------------- */
 /*เมื่อน้ำถึงระดับเซนเซอร์ จะมีเสียงบัซเซอร์ดังขึ้น */
// #define WATER_SENSOR 2 // ขา SIG ต่อกับขา D3 ของ Arduino
/* -------------------------------- */

// #define MOTION_SENSOR 7

/* RT TX  */
#define TxPin 22
#define RxPin 23
// SoftwareSerial anotherSerial(RxPin, TxPin);

void pins_init() {
  // pinMode(WATER_SENSOR, INPUT);
  // pinMode(BUZZER, OUTPUT);
  pinMode(ULTRA_SONIC_TRIG, OUTPUT);
  pinMode(ULTRA_SONIC_ECHO, INPUT);

  pinMode(SERVO_PIN, OUTPUT);
	myServo.attach(SERVO_PIN);
  myServo.write(90);

  // pinMode(MOTION_SENSOR, INPUT);
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
    float waterTemp = sensors.getTempCByIndex(0); // Read temperature in Celsius from first sensor
    waterTemp = roundf(waterTemp * 100) / 100.0;       // Round to 2 decimal places
    return waterTemp;
}

float mesureWaterLevel() {
	// Ultra Sonic Sensor
	digitalWrite(ULTRA_SONIC_TRIG, HIGH);
	delayMicroseconds(10);
	digitalWrite(ULTRA_SONIC_TRIG, LOW);

  const float EMPTY_BOTTLE_DEPTH = 14.5; // Bottle Depth When Empty 

	// measure duration of pulse from ECHO pin
	float duration_us = pulseIn(ULTRA_SONIC_ECHO, HIGH);
	float distance_cm = 0.017 * duration_us; // Distance from Ultrasonic to Water Surface

	Serial.println("Distance:" +  String(distance_cm) + " cm");

  float waterLevel = ( (EMPTY_BOTTLE_DEPTH - distance_cm) / EMPTY_BOTTLE_DEPTH ) * 100; // Water Level in Percent
  
  waterLevel = roundf(waterLevel * 100) / 100.0;   // Round to 2 decimal places

  return waterLevel;
}

/*ฟังก์ชัน เช็คระดับน้ำจากตัวเซนเซอร์ ถ้าถึงระดับเซนเซอร์ จะให้ค่า true กลับไป ถ้าไม่ถึงส่งค่า false       */

// boolean isExposedToWater() {
//     if(digitalRead(WATER_SENSOR) == LOW)
//         return true;
//     else return false;
// }

/************************************************************************/

void openDoor() {
	Serial.println("Door automatically opened.");
	myServo.write(180);  // เปิดประตู
}

void closeDoor() {
	Serial.println("Door automatically closed.");
	myServo.write(0);  // ปิดประตู
}

void testServo() {
  Serial.println("Opening door...");
  openDoor();
  delay(5000);

  Serial.println("Closing door...");
  closeDoor();
  delay(5000);
}

void lcdSetup(){
  lcd.init();
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.setCursor(0, 0); // กำหนดตำแหน่งเคอร์เซอร์ที่ แถวที่ 0 บรรทัดที่ 0
  lcd.print("ArduinoAll TEST"); //พิมพ์ข้อความ
  lcd.setCursor(2, 1); // กำหนดตำแหน่งเคอร์เซอร์ที่ แถวที่ 2 บรรทัดที่ 1
  lcd.print("kjkyuyyyy");
}

void setup() {
  Serial.begin(9600);
  sensors.begin();
  pins_init();
  closeDoor();
  LiquidCrystal_I2C lcd(0x27, 16, 2);
  lcdSetup();

  
}

void loop() {
  // Create JSON
  StaticJsonDocument<128> sensorData;

  // Prepare Sensor Data to JSON
  float waterLevel = mesureWaterLevel();
  float waterTemp = readWaterTemperature();
  sensorData["temperature"] = waterTemp;
  sensorData["waterLevel"] = waterLevel;

  // Send JSON over serial
  serializeJson(sensorData, Serial);
  Serial.println();

  // Example of opening and closing the door every 5 seconds
  // testServo();
  delay(1000);
}
