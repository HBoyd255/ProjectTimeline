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
MQTTClient mqtt_client;

#define TOPIC_TIME F("/hb_mqtt/timeline/livingroom/deadline")
#define CLIENT_ID "HB_MQTT_TIMELINE"
#define ESP_HOSTNAME F("hb-mqtt-timeline")

int global_target_time = -1;

void messageReceived(String& topic, String& payload) {
    Serial.print("incoming: ");
    Serial.print(topic);
    Serial.print(" - ");
    Serial.println(payload);

    if (payload.length() != 8) {
        Serial.println("Cannot parse Time");
        global_target_time = -2;
        return;
    }

    String hour_str = payload.substring(0, 2);
    String min_str = payload.substring(3, 5);

    int hour_int = hour_str.toInt();
    int min_int = min_str.toInt();

    global_target_time = hour_int * 60 + min_int;

    Serial.print("target_time set to ");
    Serial.println(global_target_time);
}

void connect_to_mqtt() {
    Serial.print(F("checking wifi..."));
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);
    }

    Serial.print(F("\nconnecting..."));
    while (!mqtt_client.connect(CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
        Serial.print("-");
        delay(1000);
    }

    Serial.println(F("\nconnected!"));

    mqtt_client.subscribe(TOPIC_TIME);
}

#define WHITE_LED 35

void stringFixer(String* stringToFix) {
    if (stringToFix->length() < 2) {
        *stringToFix = "0" + *stringToFix;
    }
}

void showCurrentTimeOnSevenSeg(struct tm* current_time_struct) {
    String current_minute_str = String(current_time_struct->tm_min);
    String current_hour_str = String(current_time_struct->tm_hour);

    stringFixer(&current_minute_str);
    stringFixer(&current_hour_str);

    uint8_t current_hour_msd = (current_hour_str[0]) - '0';
    uint8_t current_hour_lsd = (current_hour_str[1]) - '0';

    uint8_t current_minute_msd = (current_minute_str[0]) - '0';
    uint8_t current_minute_lsd = (current_minute_str[1]) - '0';

    setDigitsOnSevenSeg(current_hour_msd, current_hour_lsd, current_minute_msd,
                        current_minute_lsd);
}

void showTargetOnSevenSeg() {
    int target_hour = global_target_time / 60;
    int target_min = global_target_time - (target_hour * 60);

    String target_hour_str = String(target_hour);
    String target_min_str = String(target_min);

    stringFixer(&target_hour_str);
    stringFixer(&target_min_str);

    int target_hour_msd = (target_hour_str[0]) - '0';
    int target_hour_lsd = (target_hour_str[1]) - '0';

    int target_minute_msd = (target_min_str[0]) - '0';
    int target_minute_lsd = (target_min_str[1]) - '0';

    setDigitsOnSevenSeg(target_hour_msd, target_hour_lsd, target_minute_msd,
                        target_minute_lsd);
}

void do_time() {
    // I think time is provided by this lib
    // https://sourceware.org/newlib/libc.html#Timefns
    time_t current = time(nullptr);
    struct tm* current_time_struct = localtime(&current);

    int mins_since_midnight =
        (current_time_struct->tm_hour * 60) + current_time_struct->tm_min;
    int remaining_time = global_target_time - mins_since_midnight;
    int blocks_of_15 = ((remaining_time + 14) / 15);

    //     Serial.print("Time: ");
    //     Serial.println(mins_since_midnight);
    //
    //     Serial.print("Target: ");
    //     Serial.println(global_target_time);
    //
    //     Serial.print("R: ");
    //     Serial.println(remaining_time);
    //
    //     Serial.print("B: ");
    //     Serial.println(blocks_of_15);

    if (mins_since_midnight < 1) {  // Error, Cannot get time from NTP.
        blink_alternating(25, 0, 25);
        return;
    }

    if (global_target_time < 0) {  // Error, Cannot get target time from mqtt.
        blink_alternating(25, 0, 0);
        return;
    }

    showCurrentTimeOnSevenSeg(current_time_struct);

    // showTargetOnSevenSeg();

    if (blocks_of_15 <= 0) {  // Time has ran out.
        blink(25, 0, 0);
        return;
    }

    set_bar(blocks_of_15);
}

void setup() {
    Serial.begin(115200);

    setup_neopixel();

    // Turn off the built in LED
    pinMode(WHITE_LED, OUTPUT);
    digitalWrite(WHITE_LED, 0);

    WiFi.hostname(ESP_HOSTNAME);
    WiFi.begin(WIFI_NAME, WIFI_PASSWORD);

    mqtt_client.begin(MQTT_HOSTNAME, wifi_client);
    mqtt_client.onMessage(messageReceived);

    connect_to_mqtt();

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    pinMode(SEVEN_SEG_DATA, OUTPUT);
    pinMode(SEVEN_SEG_LATCH, OUTPUT);
    pinMode(SEVEN_SEG_CLOCK, OUTPUT);

    beginSevenSegThread();
}

void loop() {
    mqtt_client.loop();

    delay(10);  // <- fixes some issues with WiFi stability

    if (!mqtt_client.connected()) {
        Serial.println(F("Reconnecting To MQTT Broker"));
        connect_to_mqtt();
    }

    do_time();
}