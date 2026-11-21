#define AP_SSID "Alex"
#define AP_PASS "alexpass"
#include <Arduino.h>
#include <GyverHub.h>
GyverHub hub;
void btn_cb() {
    Serial.println("click 4");
}
void btn_cb_b(gh::Build& b) {
    Serial.print("click 5 from client ID: ");
    Serial.println(b.client.id);
}
void build(gh::Builder& b) {
    b.Button();
    b.Button();
    b.beginRow();
    b.Button();
    b.Button();
    b.endRow();
    if (b.beginRow()) {
        b.Button();
        b.Button();
        b.endRow();
    }
    {
        gh::Row r(b);
        b.Button();
        b.Button();
    }
    GH_ROW(b, 1,
           b.Button();
           b.Button(););
    {
        gh::Row r(b);
        b.Slider().size(3);
        b.Button().size(1);
        b.Button();
        b.Button();
    }
    {
        gh::Row r(b);
        {
            gh::Row r(b, 2);
            b.Button();
            b.Button();
        }
        {
            gh::Row r(b, 1);
            b.Button();
            b.Button();
        }
    }
    if (b.beginRow()) {
        b.Button().label(F("my button 1")).color(gh::Colors::Red);
        b.Button();
        b.widget.label(F("my button 2"));
        b.widget.color(gh::Colors::Blue);
        b.endRow();
    }
    if (b.beginRow()) {
        if (b.Button().click()) Serial.println("click 1");
        bool flag = 0;
        b.Button().attach(&flag);
        if (flag) Serial.println("click 2");
        gh::Flag gflag;
        b.Button().attach(&gflag);
        if (gflag) Serial.println("click 3");
        b.Button().attach(btn_cb);
        b.Button().attach(btn_cb_b);
        b.endRow();
    }
    if (b.beginRow()) {
        static int sld;
        static String inp;
        static bool sw;
        b.Slider(&sld).size(2);
        b.Input(&inp).size(2);
        if (b.Switch(&sw).size(1).click()) {
            Serial.print("switch: ");
            Serial.println(sw);
        }
        b.endRow();
    }
    if (b.beginRow()) {
        b.Label_(F("label")).size(2).value("default");
        if (b.Button().size(1).click()) {
            hub.update(F("label")).value(random(100, 500));
        }
        b.endRow();
    }
    b.Title_(F("title"));
    if (0) {
        if (b.build.isUI()) {
            Serial.println("=== UI BUILD ===");
        }
        if (b.build.isSet()) {
            Serial.println("=== SET ===");
            Serial.print("name: ");
            Serial.println(b.build.name);
            Serial.print("value: ");
            Serial.println(b.build.value);
        }
        Serial.print("client from: ");
        Serial.println(gh::readConnection(b.build.client.connection()));
        Serial.print("ID: ");
        Serial.println(b.build.client.id);
        Serial.println();
    }
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
        hub.update(F("title")).value(millis());
    }
}