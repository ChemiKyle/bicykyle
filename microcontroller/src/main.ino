// Credits:
// speed calculation
// https://github.com/sciguy14/blumbike/blob/master/blumbike_photon_firmware/blumbike_photon_firmware.ino#L151

#include <ESP8266WiFi.h>
#include <TimeLib.h>
// #include <WiFiClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "settings.h"
#include "mqtt.h"

const int HALL_PIN = D2;

float start, elapsed;
float kph, mph;     // holds calculated speed vales in metric and imperial
unsigned long last_rpm_time = 0;

// universal constants
const short SECONDS_PER_MINUTE = 60;
const short MINUTES_PER_HOUR = SECONDS_PER_MINUTE;
const float MILES_PER_KM = 0.621371;

// local constants
const unsigned int UPDATE_RATE_MS = 1000; // Update and send the new values every 1000 milliseconds (this should not be any lower than 1000
const float tire_circumference = 2.093; // circumference of a 700c tire in meters
// const float tire_circumference_imperial = tire_circumference * 0.000621371;

unsigned short revolutions = 0;

void setup() {
  Serial.begin(9600);

  // connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }
  Serial.println("Connected to wifi");
  Serial.println(WiFi.localIP());

  // connect to MQTT
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  //start now (it will be reset by the interrupt after calculating revolution time)
  start = millis();

  attachInterrupt(HALL_PIN, countRevolution, FALLING);

  // initialize pins
  pinMode(HALL_PIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, HIGH);

  Serial.println("Setup complete");
}

void loop() {
  // reconnect to MQTT if disconnect occured
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  delay(UPDATE_RATE_MS);

  digitalWrite(LED_BUILTIN, LOW);

  noInterrupts();

  calculateSpeed();

  sendJsonString();

  digitalWrite(LED_BUILTIN, HIGH);

  interrupts();
}

void debugOnRevolution() {
  int sensor_detected = !digitalRead(HALL_PIN); // sensor values are inverted
  if (sensor_detected) {
    digitalWrite(LED_BUILTIN, LOW);
    sendJsonString();
    // delay(10); // "debounce" with 10ms wait
  } else {
    digitalWrite(LED_BUILTIN, HIGH);
  }
}

ICACHE_RAM_ATTR void countRevolution() { revolutions++; }

// https://gist.github.com/virgilvox/ffe1cc08a240db9792d3
void sendJsonString() {
  char data[80];

  String time = "\"time\": " + String(now());
  String speeds =
    ", \"kph\": " + String(kph) +
    ", \"mph\": " + String(mph);

  String payload = "{" + time + speeds + "}";
  payload.toCharArray(data, (payload.length() + 1));
  client.publish(MQTT_TOPIC, data);
}

ICACHE_RAM_ATTR void calculateSpeed() {
  const double tire_rpm = SECONDS_PER_MINUTE * double(UPDATE_RATE_MS) /
             double(millis() - last_rpm_time) *
             double(revolutions);  // Compute the RPMs of the tire

  kph = MINUTES_PER_HOUR * tire_rpm * tire_circumference / 1000;
  mph = kph * MILES_PER_KM;

  last_rpm_time = millis(); // Reset the rpm clock
  revolutions = 0;          // Reset the rpm counter
}
