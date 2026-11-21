#include <Arduino.h>
#define GH_NO_HTTP
#define GH_NO_WS
#define GH_NO_MQTT
#include <GyverHub.h>
GyverHub hub;
#include <SoftwareSerial.h>
SoftwareSerial bt(2, 3);
void build(gh::Builder& b) {
    static int val;
    b.Title(F("Hello!"));
    b.Slider(&val);
}
void setup() {
    bt.begin(9600);
    hub.stream.config(&bt, gh::Connection::Bluetooth);
    hub.config(F("MyDevices"), F("AVR"), F(""));
    hub.onBuild(build);
    hub.begin();
}
void loop() {
    hub.tick();
}