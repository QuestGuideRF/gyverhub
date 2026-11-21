#include <Arduino.h>
#define AP_SSID "Alex"
#define AP_PASS "alexpass"
#define GH_NO_HTTP
#define GH_NO_WS
#define GH_NO_HTTP_TRANSFER
#include <GyverHub.h>
GyverHub hub;
#include <WebSocketsServer.h>
class HubWS : public gh::Bridge {
   public:
    HubWS(void* hub) : gh::Bridge(hub, gh::Connection::WS, GyverHub::parseHook), ws(GH_WS_PORT, "", "hub") {}
    void begin() {
        ws.onEvent([this](uint8_t num, WStype_t type, uint8_t* data, size_t len) {
            switch (type) {
                case WStype_CONNECTED:
                    setFocus();
                    break;
                case WStype_DISCONNECTED:
                    clearFocus();
                    break;
                case WStype_TEXT:
                    clientID = num;
                    parse(sutil::AnyText((char*)data, len));
                    break;
                default:
                    break;
            }
        });
        ws.begin();
    }
    void end() {
        ws.close();
    }
    void tick() {
        ws.loop();
    }
    void send(gh::BridgeData& data) {
        if (data.broadcast) {
            ws.broadcastTXT(data.text.str(), data.text.length());
        } else {
            ws.sendTXT(clientID, data.text.str(), data.text.length());
        }
    }
   private:
    WebSocketsServer ws;
    uint8_t clientID = 0;
};
HubWS ws(&hub);
#ifdef ESP8266
#include <ESP8266WebServer.h>
#define SERVER_T ESP8266WebServer
#else
#include <WebServer.h>
#define SERVER_T WebServer
#endif
#include <uri/UriGlob.h>
class HubHTTP : public gh::Bridge {
   public:
    HubHTTP(void* hub) : gh::Bridge(hub, gh::Connection::HTTP, GyverHub::parseHook), server(GH_HTTP_PORT) {}
    void begin() {
        server.on(UriGlob("/hub/*"), HTTP_GET, [this]() {
            server.setContentLength(CONTENT_LENGTH_UNKNOWN);
            server.send(200, "text/plain");
            String url = server.uri().substring(5);
            parse(url);
            server.sendContent("");
            server.client().stop();
        });
        server.begin();
        server.enableCORS(true);
    }
    void end() {
        server.stop();
    }
    void tick() {
        server.handleClient();
    }
    void send(gh::BridgeData& data) {
        if (data.text.pgm()) server.sendContent_P(data.text.str(), data.text.length());
        else server.sendContent(data.text.str(), data.text.length());
    }
    SERVER_T server;
   private:
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
    hub.addBridge(&ws);
    hub.begin();
}
void loop() {
    hub.tick();
}