#pragma once
#include <Arduino.h>
#include <StringUtils.h>
#include "core.h"
#include "hub_macro.hpp"
#include "ui/update_inline.h"
class GyverHub : public ghc::HubCore {
   public:
    GyverHub(const String& net = "", const String& name = "", const String& icon = "", uint32_t id = 0) {
        config(net, name, icon, id);
        _init();
    }
    void config(const String& net, const String& name = "", const String& icon = "", uint32_t id = 0) {
        this->prefix = net;
        this->name = name;
        this->icon = icon;
#ifdef GH_ESP_BUILD
        if (id) {
            if (id <= 0xfffff) id += 0xfffff;
            ultoa(id, this->_id, HEX);
        } else {
            uint8_t mac[6];
            WiFi.macAddress(mac);
            ultoa(*((uint32_t*)(mac + 2)), this->_id, HEX);
        }
#else
        if (id <= 0x100000) id += 0x100000;
        ultoa(id, this->_id, HEX);
#endif
    }
    void setVersion(const String& v) {
        version = v;
    }
    void setBufferSize(uint16_t size) {
        _bufsize = size;
    }
    void setPIN(uint32_t pin) {
        _pin = pin;
    }
    uint32_t getPIN() {
        return _pin;
    }
    void begin() {
        for (uint8_t i = 0; i < GH_BRIDGE_AMOUNT; i++) {
            if (bridges[i]) bridges[i]->begin();
        }
#ifndef GH_NO_FS
        gh::FS.begin();
#endif
        _running_f = true;
    }
    void end() {
        for (uint8_t i = 0; i < GH_BRIDGE_AMOUNT; i++) {
            if (bridges[i]) bridges[i]->end();
        }
        _running_f = false;
    }
    bool running() {
        return _running_f;
    }
    bool focused() {
        for (uint8_t i = 0; i < GH_BRIDGE_AMOUNT; i++) {
            if (bridges[i] && bridges[i]->getFocus()) return 1;
        }
        return 0;
    }
    bool canSend() {
        return _allow_send;
    }
    void onBuild(ghc::BuildCallback callback) {
        _build_cb = callback;
    }
    void onRequest(ghc::RequestCallback callback) {
#ifndef GH_NO_REQUEST
        _req_cb = callback;
#endif
    }
    void onInfo(ghc::InfoCallback callback) {
        _info_cb = callback;
    }
    void onCLI(ghc::CliCallback callback) {
        _cli_cb = callback;
    }
    void onUnix(ghc::UnixCallback callback) {
        _unix_cb = callback;
    }
#ifdef GH_ESP_BUILD
    void onFetch(ghc::FetchCallback callback) {
        _fetch_cb = callback;
    }
    void onReboot(ghc::RebootCallback callback) {
        _reboot_cb = callback;
    }
    void onUpload(ghc::UploadCallback callback) {
        _upload_cb = callback;
    }
    void safeUpload(bool state) {
        _safe_upl = state;
    }
#endif
    void sendCLI(GHTREF str, gh::Colors col = gh::Colors::Default, gh::Client* client = nullptr) {
        if (!focused() || !_allow_send) return;
        ghc::Packet p(50);
        p.beginPacket(id, client);
        p.addString(ghc::Tag::type, ghc::Tag::print);
        p.addStringEsc(ghc::Tag::text, str);
        p.addInt(ghc::Tag::color, (uint32_t)col);
        p.endPacket();
        _send(p, client);
    }
    void sendRefresh(gh::Client* client = nullptr) {
        if (!focused() || !_allow_send) return;
        ghc::Packet p(50);
        p.beginPacket(id, client);
        p.addString(ghc::Tag::type, ghc::Tag::refresh);
        p.endPacket();
        _send(p, client);
    }
    void sendScript(GHTREF script, gh::Client* client = nullptr) {
        if (!focused() || !_allow_send) return;
        ghc::Packet p(50);
        p.beginPacket(id, client);
        p.addString(ghc::Tag::type, ghc::Tag::script);
        p.addStringEsc(ghc::Tag::script, script);
        p.endPacket();
        _send(p, client);
    }
    void sendAction(GHTREF name, gh::Client* client = nullptr) {
        if (!focused() || !_allow_send) return;
        ghc::Packet p(50);
        p.beginPacket(id, client);
        p.addString(ghc::Tag::type, ghc::Tag::update);
        p.beginObj(ghc::Tag::updates);
        p.addKey(name);
        p.addTextRaw(F("{\"action\":1}}"));
        p.endPacket();
        _send(p, client);
    }
    gh::UpdateInline update(GHTREF name, GHTREF func, gh::Client* client = nullptr) {
        return gh::UpdateInline(canSend(), _id, name, func, client ? *client : gh::Client(this, sendHook));
    }
    gh::UpdateInline update(GHTREF name, gh::Client* client = nullptr) {
        return update(name, GHTXT(), client);
    }
    void sendUpdate(GHTREF name, const su::Value& value, gh::Client* client = nullptr) {
        if (!focused() || !canSend()) return;
        _sendUpdate(name, value, client);
    }
    void sendUpdate(GHTREF name, double value, uint8_t dec, gh::Client* client = nullptr) {
        sendUpdate(name, su::Value(value, dec), client);
    }
    void sendUpdate(String name, gh::Client* client = nullptr) {
        if (!_build_cb || !focused() || _build_busy) return;
        gh::Client read_client;
        ghc::Packet p(50);
        p.beginPacket(id, client);
        p.addString(ghc::Tag::type, ghc::Tag::update);
        p.beginObj(ghc::Tag::updates);
        for (sutil::Parser pr(name); pr.next();) {
            p.addKey(pr.str());
            p.s += F("{\"ценить\":\"");
            p.reserve(p.length() + 30);
            _readBuild(p, read_client, pr.str());
            p.s += F("\"},");
        }
        p.endObj();
        p.endPacket();
        _send(p, client);
    }
    void sendGetAuto(bool state) {
        _autoGet_f = state;
    }
    void sendGet(GHTREF name, const su::Value& value) {
        _sendGet(name, value);
    }
    void sendGet(GHTREF name, double value, uint8_t dec) {
        sendGet(name, su::Value(value, dec));
    }
    void sendGet(String name) {
        if (!_running_f || !_build_cb || _build_busy) return;
        gh::Client client;
        ghc::Packet p(30);
        for (sutil::Parser pr(name); pr.next();) {
            bool ok = _readBuild(p, client, pr.str());
            if (ok) _sendGet(pr.str(), p.s);
        }
    }
    void sendStatus(bool status) {
        String topic = _topicStatus();
        _sendMQTT(topic, status ? F("online") : F("offline"));
    }
    String topicStatus() {
        String t(prefix);
        t += F("/hub/");
        t += id;
        t += F("/status");
        return t;
    }
    String topicDiscover() {
        return prefix;
    }
    String topicHub() {
        String t(prefix);
        t += '/';
        t += id;
        t += "/#";
        return t;
    }
    void sendPush(GHTREF text, gh::Client* client = nullptr) {
        if (!_running_f || !_allow_send) return;
        ghc::Packet p(50);
        p.beginPacket(id, client);
        p.addString(ghc::Tag::type, ghc::Tag::push);
        p.addStringEsc(ghc::Tag::text, text);
        p.endPacket();
        _send(p, client);
    }
    void sendNotice(GHTREF text, gh::Colors col = gh::Colors::Green, gh::Client* client = nullptr) {
        if (!_running_f || !_allow_send) return;
        ghc::Packet p(50);
        p.beginPacket(id, client);
        p.addString(ghc::Tag::type, ghc::Tag::notice);
        p.addStringEsc(ghc::Tag::text, text);
        p.addInt(ghc::Tag::color, (uint32_t)col);
        p.endPacket();
        _send(p, client);
    }
    void sendAlert(GHTREF text, gh::Client* client = nullptr) {
        if (!_running_f || !_allow_send) return;
        ghc::Packet p(50);
        p.beginPacket(id, client);
        p.addString(ghc::Tag::type, ghc::Tag::alert);
        p.addStringEsc(ghc::Tag::text, text);
        p.endPacket();
        _send(p, client);
    }
    String getUI(bool enclose = false) {
        if (_build_busy) return String();
        gh::Client client;
        ghc::Packet p;
        if (enclose) p.beginArr();
        _uiBuild(p, client);
        if (enclose) p.endArr();
        p.end();
        return p.s;
    }
    String getValues() {
        if (_build_busy) return String();
        ghc::Packet p(50);
        p.beginObj();
        if (_build_cb) {
            _build_busy = true;
            _allow_send = false;
            gh::Client client;
            gh::Build build(gh::Action::Get, client);
            gh::Builder b(build, &p);
            _build_cb(b);
            _allow_send = true;
            _build_busy = false;
        }
        p.endObj();
        p.end();
        return p.s;
    }
    String getValue(GHTREF name) {
        if (_build_busy) return String();
        gh::Client client;
        ghc::Packet p;
        _readBuild(p, client, name);
        return p.s;
    }
    bool tick() {
#ifdef GH_ESP_BUILD
#ifndef GH_NO_FS
#ifndef GH_NO_FETCH
        if (_fet_p && _fet_p->timeout()) GHDELPTR(_fet_p);
#endif
#ifndef GH_NO_UPLOAD
        if (_upl_p && _upl_p->timeout()) GHDELPTR(_upl_p);
#endif
#endif
#ifndef GH_NO_OTA
        if (_ota_p && _ota_p->timeout()) GHDELPTR(_ota_p);
#endif
        if (_reason != gh::Reboot::None) {
            if (_reboot_cb) _reboot_cb(_reason);
            delay(GH_REBOOT_DELAY);
            ESP.restart();
        }
#ifndef GH_NO_OTA_URL
        if (_otaurl_p) {
            if (_otaurl_p->update()) {
                _reason = gh::Reboot::OtaUrl;
            }
            GHDELPTR(_otaurl_p);
        }
#endif
#endif
        if (_running_f) {
            for (uint8_t i = 0; i < GH_BRIDGE_AMOUNT; i++) {
                if (bridges[i] && bridges[i]->state()) bridges[i]->tick();
            }
            return 1;
        }
        return 0;
    }
   private:
};