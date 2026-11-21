#include <Arduino.h>
#define AP_SSID "Alex"
#define AP_PASS "alexpass"
#define GH_NO_HTTP
#define GH_NO_WS
#define GH_NO_HTTP_TRANSFER
#include <GyverHub.h>
GyverHub hub;
#include <ESP8266WiFi.h>
class HubHTTP : public gh::Bridge {
   public:
    HubHTTP(void* hub) : gh::Bridge(hub, gh::Connection::HTTP, GyverHub::parseHook), server(GH_HTTP_PORT) {}
    void begin() {
        server.begin();
    }
    void end() {
        server.stop();
    }
    void tick() {
        WiFiClient client = server.accept();
        if (client) {
            String req = client.readStringUntil('/');
            req = client.readStringUntil('\r');
            while (client.available()) client.read();
            int start = req.indexOf(F("hub/"));
            int end = req.indexOf(F(" HTTP/"));
            if (start < 0 || end < 0) return;
            req = req.substring(start + 4, end);
            client.print(F(
                "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n"
                "Access-Control-Allow-Origin:*\r\n"
                "Access-Control-Allow-Private-Network: true\r\n"
                "Access-Control-Allow-Methods:*\r\n\r\n"));
            client_p = &client;
            parse(req);
            client_p = nullptr;
        }
    }
    void send(gh::BridgeData& data) {
        if (client_p) client_p->print(data.text);
    }
    WiFiServer server;
   private:
    WiFiClient* client_p = nullptr;
};
HubHTTP http(&hub);
void build(gh::Builder& b) {
    static int val;
    b.Title(F("Hello!"));
    b.Slider(&val);
}
void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(AP_SSID, AP_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println(WiFi.localIP());
    hub.config(F("MyDevices"), F("ESP"), F(""));
    hub.onBuild(build);
    hub.addBridge(&http);
    hub.begin();
}
void loop() {
    hub.tick();
}