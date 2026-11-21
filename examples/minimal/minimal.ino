#include <Arduino.h>
#define AP_SSID ""
#define AP_PASS ""
#include <GyverHub.h>
GyverHub hub;
void build(gh::Builder& b) {
}
void setup() {
    Serial.begin(115200);
#ifdef GH_ESP_BUILD
    WiFi.mode(WIFI_STA);
    WiFi.begin(AP_SSID, AP_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println(WiFi.localIP());
    hub.mqtt.config("test.mosquitto.org", 1883);
#endif
    hub.config(F("MyDevices"), F("ESP"));
    hub.onBuild(build);
    hub.begin();
}
void loop() {
    hub.tick();
}