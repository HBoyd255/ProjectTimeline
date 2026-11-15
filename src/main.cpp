#include <Arduino.h>
#include <MQTT.h>
#include <WiFi.h>

#include "sevenSegment.h"
#include "wifiCredentials.h"

TaskHandle_t Task2;

// Code Modified from
// https://forum.arduino.cc/t/simple-example-for-time-via-internet-esp32/627654/2

const char* ntpServer = "homeassistant.local";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 3600;

WiFiClient wifi_client;
MQTTClient mqtt_client;

#include <NeoPixelBus.h>

#define LED_COUNT 8
#define DATA_PIN 48

#define TOPIC_TIME F("/hb_mqtt/time_line/livingroom/target_time")
#define CLIENT_ID "HB_MQTT_TIME_LINE"
#define ESP_HOSTNAME F("hb-mqtt-time-line")

NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> ledStrip(LED_COUNT, DATA_PIN);

int global_target_time = -1;

const uint8_t fade_animation[255] = {
    2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  4,  4,
    5,  5,  6,  7,  7,  8,  9,  9,  10, 11, 12, 13, 13, 14, 15, 16, 17, 18, 19,
    20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 31, 32, 33, 34, 35, 36, 38, 39, 40,
    41, 42, 44, 45, 46, 47, 49, 50, 51, 52, 53, 55, 56, 57, 58, 60, 61, 62, 63,
    64, 66, 67, 68, 69, 70, 71, 72, 73, 74, 76, 77, 78, 79, 80, 81, 82, 82, 83,
    84, 85, 86, 87, 88, 88, 89, 90, 91, 91, 92, 93, 93, 94, 94, 95, 95, 96, 96,
    97, 97, 98, 98, 98, 98, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 98, 98, 98, 98, 97, 97, 96, 96, 95, 95, 94, 94, 93, 93, 92, 91,
    91, 90, 89, 88, 88, 87, 86, 85, 84, 83, 82, 82, 81, 80, 79, 78, 77, 76, 74,
    73, 72, 71, 70, 69, 68, 67, 66, 64, 63, 62, 61, 60, 58, 57, 56, 55, 53, 52,
    51, 50, 49, 47, 46, 45, 44, 42, 41, 40, 39, 38, 36, 35, 34, 33, 32, 31, 29,
    28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 13, 12, 11,
    10, 9,  9,  8,  7,  7,  6,  5,  5,  4,  4,  3,  3,  2,  2,  2,  2,  2,  2,
    2,  2,  2,  2,  2,  2,  2,  2};

void blink_alternating(uint8_t r, uint8_t g, uint8_t b) {
    bool toggle = (millis() >> 8) & 1;  // Toggle each 0.25 ish secconds.

    if (toggle) {
        ledStrip.SetPixelColor(0, RgbColor(r, g, b));
        ledStrip.SetPixelColor(1, RgbColor(0, 0, 0));
        ledStrip.SetPixelColor(2, RgbColor(r, g, b));
        ledStrip.SetPixelColor(3, RgbColor(0, 0, 0));
        ledStrip.SetPixelColor(4, RgbColor(r, g, b));
        ledStrip.SetPixelColor(5, RgbColor(0, 0, 0));
        ledStrip.SetPixelColor(6, RgbColor(r, g, b));
        ledStrip.SetPixelColor(7, RgbColor(0, 0, 0));

    } else {
        ledStrip.SetPixelColor(0, RgbColor(0, 0, 0));
        ledStrip.SetPixelColor(1, RgbColor(r, g, b));
        ledStrip.SetPixelColor(2, RgbColor(0, 0, 0));
        ledStrip.SetPixelColor(3, RgbColor(r, g, b));
        ledStrip.SetPixelColor(4, RgbColor(0, 0, 0));
        ledStrip.SetPixelColor(5, RgbColor(r, g, b));
        ledStrip.SetPixelColor(6, RgbColor(0, 0, 0));
        ledStrip.SetPixelColor(7, RgbColor(r, g, b));
    }
    ledStrip.Show();
}

void blink(uint8_t r, uint8_t g, uint8_t b) {
    bool toggle = (millis() >> 8) & 1;  // Toggle each 0.25 ish secconds.

    if (toggle) {
        ledStrip.SetPixelColor(0, RgbColor(r, g, b));
        ledStrip.SetPixelColor(1, RgbColor(r, g, b));
        ledStrip.SetPixelColor(2, RgbColor(r, g, b));
        ledStrip.SetPixelColor(3, RgbColor(r, g, b));
        ledStrip.SetPixelColor(4, RgbColor(r, g, b));
        ledStrip.SetPixelColor(5, RgbColor(r, g, b));
        ledStrip.SetPixelColor(6, RgbColor(r, g, b));
        ledStrip.SetPixelColor(7, RgbColor(r, g, b));

    } else {
        ledStrip.SetPixelColor(0, RgbColor(0, 0, 0));
        ledStrip.SetPixelColor(1, RgbColor(0, 0, 0));
        ledStrip.SetPixelColor(2, RgbColor(0, 0, 0));
        ledStrip.SetPixelColor(3, RgbColor(0, 0, 0));
        ledStrip.SetPixelColor(4, RgbColor(0, 0, 0));
        ledStrip.SetPixelColor(5, RgbColor(0, 0, 0));
        ledStrip.SetPixelColor(6, RgbColor(0, 0, 0));
        ledStrip.SetPixelColor(7, RgbColor(0, 0, 0));
    }
    ledStrip.Show();
}

void setBar(uint8_t level) {
    uint8_t animation_frame =
        (millis() & 4095) >> 4;  // Toggle each 0.25 ish secconds.

    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;

    uint8_t animation_r = 0;
    uint8_t animation_g = 0;
    uint8_t animation_b = 0;

    if (level > 4) {
        g = 25;
        animation_g = fade_animation[animation_frame];
    } else if (level > 2) {
        r = 25;
        g = 12;
        animation_r = fade_animation[animation_frame];
        animation_g = fade_animation[animation_frame] >> 1;
    } else {
        r = 25;
        animation_r = fade_animation[animation_frame];
    }

    for (uint8_t i = 0; i < 8; i++) {
        if (i == (level - 1)) {
            ledStrip.SetPixelColor(
                i, RgbColor(animation_r, animation_g, animation_b));

        } else if (i < (level - 1)) {
            ledStrip.SetPixelColor(i, RgbColor(r, g, b));
        } else {
            ledStrip.SetPixelColor(i, RgbColor(0, 0, 0));
        }
    }

    ledStrip.Show();
}

void messageReceived(String& topic, String& payload) {
    Serial.println("incoming: " + topic + " - " + payload);

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

    Serial.println("target_time set to " + global_target_time);
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

    showCurrentTimeOnSevenSeg();

    // showTargetOnSevenSeg();

    if (blocks_of_15 <= 0) {  // Time has ran out.
        blink(25, 0, 0);
        return;
    }

    setBar(blocks_of_15);
}

void setup() {
    Serial.begin(115200);

    ledStrip.Begin();

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