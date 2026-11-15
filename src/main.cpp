#include <Arduino.h>
#include <MQTT.h>
#include <WiFi.h>

#include "neopixelWrapper.h"
#include "sevenSegment.h"
#include "wifiCredentials.h"

// Code Modified from
// https://forum.arduino.cc/t/simple-example-for-time-via-internet-esp32/627654/2

const char* ntpServer = "homeassistant.local";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 3600;

WiFiClient wifi_client;
MQTTClient mqttClient;

#define TOPIC_TIME F("/hb_mqtt/timeline/livingroom/deadline")
#define CLIENT_ID "HB_MQTT_TIMELINE"
#define ESP_HOSTNAME F("hb-mqtt-timeline")

int targetTime_G = -1;

void messageReceived(String& topic, String& payload) {
    Serial.print("incoming: ");
    Serial.print(topic);
    Serial.print(" - ");
    Serial.println(payload);

    if (payload.length() != 8) {
        Serial.println("Cannot parse Time");
        targetTime_G = -2;
        return;
    }

    String hour_str = payload.substring(0, 2);
    String min_str = payload.substring(3, 5);

    int hour_int = hour_str.toInt();
    int min_int = min_str.toInt();

    targetTime_G = hour_int * 60 + min_int;

    Serial.print("target_time set to ");
    Serial.println(targetTime_G);
}

void connectToMqtt() {
    Serial.print(F("checking wifi..."));
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);
    }

    Serial.print(F("\nconnecting..."));
    while (!mqttClient.connect(CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
        Serial.print("-");
        delay(1000);
    }

    Serial.println(F("\nconnected!"));

    mqttClient.subscribe(TOPIC_TIME);
}

#define WHITE_LED 35

void stringFixer(String* stringToFix) {
    if (stringToFix->length() < 2) {
        *stringToFix = "0" + *stringToFix;
    }
}

void showCurrentTimeOnSevenSeg(struct tm* currentTimeStruct) {
    String currentMinute_str = String(currentTimeStruct->tm_min);
    String currentHour_str = String(currentTimeStruct->tm_hour);

    stringFixer(&currentMinute_str);
    stringFixer(&currentHour_str);

    uint8_t currentHourMsd = (currentHour_str[0]) - '0';
    uint8_t currentHourLsd = (currentHour_str[1]) - '0';

    uint8_t currentMinuteMsd = (currentMinute_str[0]) - '0';
    uint8_t currentMinuteLsd = (currentMinute_str[1]) - '0';

    setDigitsOnSevenSeg(currentHourMsd, currentHourLsd, currentMinuteMsd,
                        currentMinuteLsd);
}

void showTargetOnSevenSeg() {
    int targetHour = targetTime_G / 60;
    int targetMin = targetTime_G - (targetHour * 60);

    String targetHour_str = String(targetHour);
    String targetMin_str = String(targetMin);

    stringFixer(&targetHour_str);
    stringFixer(&targetMin_str);

    int targetHourMsd = (targetHour_str[0]) - '0';
    int targetHourLsd = (targetHour_str[1]) - '0';

    int targetMinuteMsd = (targetMin_str[0]) - '0';
    int targetMinuteLsd = (targetMin_str[1]) - '0';

    setDigitsOnSevenSeg(targetHourMsd, targetHourLsd, targetMinuteMsd,
                        targetMinuteLsd);
}

void doTime() {
    // I think time is provided by this lib
    // https://sourceware.org/newlib/libc.html#Timefns
    time_t current = time(nullptr);
    struct tm* currentTimeStruct = localtime(&current);

    int minsSinceMidnight =
        (currentTimeStruct->tm_hour * 60) + currentTimeStruct->tm_min;
    int remainingTime = targetTime_G - minsSinceMidnight;
    int blocksOf15 = ((remainingTime + 14) / 15);

    //     Serial.print("Time: ");
    //     Serial.println(minsSinceMidnight);
    //
    //     Serial.print("Target: ");
    //     Serial.println(targetTime_G);
    //
    //     Serial.print("R: ");
    //     Serial.println(remainingTime);
    //
    //     Serial.print("B: ");
    //     Serial.println(blocksOf15);

    if (minsSinceMidnight < 1) {  // Error, Cannot get time from NTP.
        blinkAlternating(25, 0, 25);
        return;
    }

    if (targetTime_G < 0) {  // Error, Cannot get target time from mqtt.
        blinkAlternating(25, 0, 0);
        return;
    }

    showCurrentTimeOnSevenSeg(currentTimeStruct);

    // showTargetOnSevenSeg();

    if (blocksOf15 <= 0) {  // Time has ran out.
        blink(25, 0, 0);
        return;
    }

    setBar(blocksOf15);
}

void setup() {
    Serial.begin(115200);

    setupNeopixel();

    // Turn off the built in LED
    pinMode(WHITE_LED, OUTPUT);
    digitalWrite(WHITE_LED, 0);

    WiFi.hostname(ESP_HOSTNAME);
    WiFi.begin(WIFI_NAME, WIFI_PASSWORD);

    mqttClient.begin(MQTT_HOSTNAME, wifi_client);
    mqttClient.onMessage(messageReceived);

    connectToMqtt();

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    pinMode(SEVEN_SEG_DATA, OUTPUT);
    pinMode(SEVEN_SEG_LATCH, OUTPUT);
    pinMode(SEVEN_SEG_CLOCK, OUTPUT);

    beginSevenSegThread();
}

void loop() {
    mqttClient.loop();

    delay(10);  // <- fixes some issues with WiFi stability

    if (!mqttClient.connected()) {
        Serial.println(F("Reconnecting To MQTT Broker"));
        connectToMqtt();
    }

    doTime();
}