#pragma once
#include <Arduino.h>
#include <StringUtils.h>
#include "builder_class.h"
#include "core/anyptr.h"
#include "core/build.h"
#include "core/datatypes.h"
#include "core/packet.h"
#include "core/types.h"
#include "flag.h"
#include "hub_macro.hpp"
#include "update_class.h"
namespace ghc {
class Widget {
    friend class gh::Builder;
    friend class gh::Update;
   public:
    Widget(ghc::Packet& p, gh::Build* build = nullptr) : p(p), _build(build) {
        _allowed = (!_build || build->action == gh::Action::UI);
    }
    Widget& param(GHTREF key, const sutil::AnyValue& val) {
        if (_ok() && val.valid()) {
            if (val.type() == sutil::AnyText::Type::value) p.addInt(key, val);
            else p.addStringEsc(key, val);
        }
        return *this;
    }
    Widget& param(GHTREF key, double val, uint8_t dec) {
        if (_ok()) p.addFloat(key, val, dec);
        return *this;
    }
    Widget& valueVar(const ghc::AnyPtr& data) {
        if (_ok() && data.ptr) {
            p.addKey(Tag::value);
            p.quotes();
            ghc::varToStr(p, data);
            p.quotes();
            p.comma();
        }
        return *this;
    }
    Widget& value(const sutil::AnyValue& val) {
        return param(Tag::value, val);
    }
    Widget& value(double val, uint8_t dec) {
        return param(Tag::value, val, dec);
    }
    Widget& text(GHTREF str) {
        return param(Tag::text, str);
    }
    Widget& icon(GHTREF str) {
        return param(Tag::icon, str);
    }
    Widget& maxLen(uint16_t len) {
        return param(Tag::maxlen, len);
    }
    Widget& rows(uint16_t rows) {
        return param(Tag::rows, rows);
    }
    Widget& regex(GHTREF str) {
        return param(Tag::regex, str);
    }
    Widget& align(gh::Align align) {
        return param(Tag::align, (uint8_t)align);
    }
    Widget& range(double min, double max, double step, uint8_t dec = 2) {
        if (_ok()) {
            p.addFloat(Tag::min, min, dec);
            p.addFloat(Tag::max, max, dec);
            p.addFloat(Tag::step, step, dec);
            p.addInt(Tag::dec, dec);
        }
        return *this;
    }
    template <typename T>
    Widget& range(T min, T max, T step) {
        if (_ok()) {
            p.addInt(Tag::min, min);
            p.addInt(Tag::max, max);
            p.addInt(Tag::step, step);
            p.addInt(Tag::dec, 0);
        }
        return *this;
    }
    Widget& unit(GHTREF str) {
        return param(Tag::unit, str);
    }
    Widget& fontSize(uint16_t size) {
        return param(Tag::fsize, size);
    }
    Widget& color(uint32_t color) {
        if (color != 0xffffffff) return param(Tag::color, color);
        return *this;
    }
    Widget& color(gh::Colors col) {
        return color((uint32_t)col);
    }
    Widget& action(bool act = 1) {
        return param(Tag::action, act);
    }
    Widget& size(uint16_t width, uint16_t height = 0) {
        if (_ok()) {
            p.addInt(Tag::wwidth, width);
            if (height) p.addInt(Tag::wheight, height);
        }
        return *this;
    }
    Widget& label(GHTREF str) {
        return param(Tag::label, str);
    }
    Widget& noLabel(bool nolabel = true) {
        return param(Tag::nolabel, nolabel);
    }
    Widget& suffix(GHTREF str) {
        return param(Tag::suffix, str);
    }
    Widget& noTab(bool notab = true) {
        return param(Tag::notab, notab);
    }
    Widget& square(bool square = true) {
        return param(Tag::square, square);
    }
    Widget& disabled(bool disable = true) {
        return param(Tag::disable, disable);
    }
    Widget& hint(GHTREF str) {
        return param(Tag::hint, str);
    }
    bool click() {
        return _click;
    }
    Widget& attach(void (*cb)()) {
        if (_click && cb) cb();
        return *this;
    }
    Widget& attach(void (*cb)(gh::Build& build)) {
        if (_click && cb && _build) cb(*_build);
        return *this;
    }
    Widget& attach(void (*cb)(gh::Builder& build), gh::Builder& b) {
        if (_click && cb && _build) cb(b);
        return *this;
    }
    Widget& attach(gh::Flag* flag) {
        if (_click) flag->_changed = true;
        return *this;
    }
    Widget& attach(bool* flag) {
        if (_click) *flag = true;
        return *this;
    }
   protected:
    ghc::Packet& p;
    gh::Build* _build;
    bool _enabled = true;
    bool _allowed = false;
    bool _click = false;
    bool _ok() {
        return _enabled && _allowed;
    }
    Widget& param(Tag key, const sutil::AnyValue& val) {
        if (_ok() && val.valid()) {
            if (val.type() == sutil::AnyText::Type::value) p.addInt(key, val);
            else p.addStringEsc(key, val);
        }
        return *this;
    }
    Widget& param(Tag key, double val, uint8_t dec) {
        if (_ok()) p.addFloat(key, val, dec);
        return *this;
    }
};
}