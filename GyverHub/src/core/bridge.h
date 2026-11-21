#pragma once
#include <Arduino.h>
#include "bridge_class.h"
#include "hooks.h"
#include "hub_macro.hpp"
#include "types.h"
#define GH_CONN_TOUT 3500
namespace gh {
struct BridgeData {
    BridgeData(GHTREF text) : text(text) {}
    GHTREF text;
    String topic;
    bool broadcast = 0;
};
class Bridge {
   public:
    Bridge() {}
    Bridge(void* hub, Connection conn, ghc::ParseHook parse) {
        config(hub, conn, parse);
    }
    void config(void* hub, Connection conn, ghc::ParseHook parse) {
        _hub = hub;
        _conn = conn;
        _parse = parse;
    }
    virtual void begin(){};
    virtual void end(){};
    virtual void tick(){};
    virtual void send(BridgeData& data){};
    void parse(GHTXT url, GHTXT data = GHTXT()) {
        if (!_state || !url.valid()) return;
        if (!data.valid()) {
            char* eq = (char*)url.find('=');
            if (eq) {
                *eq = 0;
                data = GHTXT(eq + 1, url.end() - eq - 1);
                url = GHTXT(url.str(), eq - url.str());
            }
        }
        _parse(_hub, *this, url, data);
    }
    Connection connection() {
        return _conn;
    }
    void enable() {
        _state = true;
    }
    void disable() {
        _state = false;
    }
    bool state() {
        return _state;
    }
    void setFocus() {
        _focus_tmr = millis();
    }
    void clearFocus() {
        _focus_tmr = millis() - GH_CONN_TOUT;
    }
    bool getFocus() {
        return (millis() - _focus_tmr < GH_CONN_TOUT);
    }
   protected:
    void* _hub;
    Connection _conn;
    ghc::ParseHook _parse;
   private:
    uint32_t _focus_tmr = 0;
    bool _state = true;
};
}