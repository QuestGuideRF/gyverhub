#include <Arduino.h>
#define AP_SSID "Alex"
#define AP_PASS "alexpass"
#include <GyverHub.h>
GyverHub hub;
#include <PairsFile.h>
PairsFile data(&GH_FS, "/data.dat", 3000);
void build(gh::Builder& b) {
    b.Input_("input", &data);
    {
        gh::Row r(b);
        b.Slider_("slider", &data);
        b.Spinner_("spinner", &data);
        b.Switch_("switch", &data);
    }
    b.Text_("pairs", data);
    if (b.changed()) hub.update("pairs").value(data);
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
    data.begin();
}
void loop() {
    hub.tick();
    data.tick();
}