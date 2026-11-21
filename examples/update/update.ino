#include <Arduino.h>
#define AP_SSID "Alex"
#define AP_PASS "alexpass"
#include <GyverHub.h>
GyverHub hub;
void build(gh::Builder& b) {
    if (b.beginRow()) {
        b.Input_("inp").size(3).value("hello").color(gh::Colors::Blue);
        if (b.Button().size(1).label("value").click()) hub.update("inp").value(random(1000));
        if (b.Button().label("color").click()) hub.update("inp").color(gh::Colors::Red);
        b.endRow();
    }
    if (b.beginRow()) {
        b.Slider_("sld").size(3);
        if (b.Button().size(2).label("update").click()) {
            hub.update("sld").value(random(100)).color(gh::Colors::Pink).unit("°");
        }
        b.endRow();
    }
    if (b.beginRow()) {
        b.Label_("lbl").size(3);
        if (b.Button().label("all").size(1).click()) hub.update("lbl").value("all");
        if (b.Button().label("client").click()) hub.update("lbl", &b.build.client).value("client");
        b.endRow();
    }
    if (b.Button().label("refresh").click()) b.refresh();
    b.Title_("tit").value("Title");
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
    hub.config(F("MyDevices"), F("ESP"), F(""));
    hub.onBuild(build);
    hub.begin();
}
void loop() {
    hub.tick();
    static gh::Timer tmr(1000);
    if (tmr) {
        hub.update("tit").value(millis());
    }
    static gh::Timer tmr2(5000);
    if (tmr2) {
        gh::Update upd(&hub);
        upd.update("name1").value(random(100));
        upd.update("name2").color(gh::Colors::Red).label("kek");
        upd.send();
    }
}