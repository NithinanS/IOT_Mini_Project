#define BLYNK_TEMPLATE_ID           "TMPL6iTgMpxXr"
#define BLYNK_TEMPLATE_NAME         "miniProj"
#define BLYNK_AUTH_TOKEN            "jRqpqDZlUCBdlLsynkT_ENtKSx38b4bA"

#include <Arduino.h>
#include <BlynkSimpleEsp32.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <SoftwareSerial.h>
#include <nlohmann/json.hpp>

//Pins constant
#define TxPin = 22;
#define RxPin = 23;
#define MOTION_SENSOR = 5;
#define SERVO_PIN = 18; //TODO: change pin?

//Variables
char ssid[] = "Dami 14";
char password[] = "BigMi1414";
SoftwareSerial anotherSerial(RxPin, TxPin);
BlynkTimer timer;

const float TANK_HEIGHT = 15;
int opened;
float temperature = 0;
float volume = 0;

void updateBlynk();
void processCommand(String);
float levelToVolume(float dist);

BLYNK_CONNECTED() {
  Serial.println("Blynk connection success");
}

BLYNK_WRITE(V0) {
  opened = param.asInt()
}

void setup() {
  Serial.begin(9600);
  anotherSerial.begin(19200);

  /*****************WI-FI*****************/
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting To WiFi Network .");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println("\nConnected To The WiFi Network");

  /*****************Blynk*****************/
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
  timer.setInterval(10000L, updateBlynk);


  pinMode(MOTION_SENSOR, INPUT);
  pinMode(SERVO_PIN, OUTPUT);
}

void loop() {
  Blynk.run();
  timer.run();

  //check serial input
  if (anotherSerial.available() > 0) {
    String sensorOutput = anotherSerial.readStringUntil('\n');
    processCommand(sensorOutput);
  }
}

void processCommand(String command) { //command format : "d=1" or "t=26"
    json j = json::parse(command);

    // Convert to std::map<std::string, double>
    std::map<std::string, double> sensorMap;
    for (auto& el : j.items()) {
        sensorMap[el.key()] = el.value();
    }

    temperature = sensormap["temperature"];
    volume = levelToVolume(sensormap["distance"])
}

float levelToVolume(float dist) {
    min(round((TANK_HEIGHT - dist)*100/TANK_HEIGHT, 2), 100)
}

void updateBlynk() {
  Serial.println("update blynk");
  // Serial.print("temp : ");
  // Serial.println(temperature);
  // Serial.print("humidity : ");
  // Serial.println(humidity);
  Blynk.virtualWrite(V0, opened);
}