#pragma once
#include <Arduino.h>
#include <StringUtils.h>
#ifndef GH_NO_PAIRS
#include <Pairs.h>
#endif
#include "builder_class.h"
#include "builder_macro.h"
#include "button.h"
#include "canvas.h"
#include "core/build.h"
#include "core/client.h"
#include "core/core_class.h"
#include "core/datatypes.h"
#include "core/hooks.h"
#include "core/hub_class.h"
#include "core/packet.h"
#include "core/types.h"
#include "flags.h"
#include "hub_macro.hpp"
#include "log.h"
#include "widget.h"
namespace gh {
class Builder {
    friend class ghc::HubCore;
    friend class ::GyverHub;
   public:
    Build& build;
    ghc::Widget widget;
    Builder(Build& build,
            ghc::Packet* p = nullptr) : build(build),
                                        widget(*p, &build),
                                        p(p) {}
    void refresh() {
        _refresh = true;
    }
    bool isRefresh() {
        return _refresh;
    }
    bool show(bool en = true) {
        widget._enabled = en;
        return true;
    }
    bool changed() {
        return _changed;
    }
    bool click() {
        return widget.click();
    }
    void addJSON(GHTREF text) {
        if (p && build.action == Action::UI) {
            if (_checkFirst()) p->endObj();
            p->sendText(text);
            p->comma();
            p->beginObj();
        }
    }
    void addJSON_P(PGM_P text) {
        addJSON((FSTR)text);
    }
    void addJSON_File(GHTREF path) {
        _checkName();
        switch (build.action) {
            case Action::UI:
                _beginName();
                _tag(ghc::Tag::ui_file);
                widget.value(path);
                break;
            default:
                break;
        }
    }
    uint8_t menu() {
        return _menu ? *_menu : 0;
    }
    GH_BUILD_CONTAINER(Row, ghc::Tag::row);
    GH_BUILD_CONTAINER(Col, ghc::Tag::col);
    ghc::Widget& Space() {
        return _varAndType(ghc::Tag::space, GHTXT(), ghc::AnyPtr());
    }
    ghc::Widget& Hook_(GHTREF name, const ghc::AnyPtr& data, GHTREF func = GHTXT()) {
        _checkName(name);
        switch (build.action) {
            case Action::UI:
                if (_checkFirst()) p->endObj();
                p->beginObj();
                p->addString(ghc::Tag::type, ghc::Tag::hook);
                p->addString(ghc::Tag::id, name);
                _func(func);
                _uiHelper(name, data);
                break;
            case Action::Get:
                _getHelper(name, data);
                break;
            case Action::Read:
                if (_readName(name) && data.ptr) _writeValue(name, data);
                break;
            case Action::Set:
                _setHelper(name, data);
                break;
            default:
                break;
        }
        return widget;
    }
    GH_BUILD_VAR(Input, ghc::Tag::input);
    GH_BUILD_VAR(InputArea, ghc::Tag::area);
    GH_BUILD_VAR(Pass, ghc::Tag::pass);
    GH_BUILD_VAR(Date, ghc::Tag::date);
    GH_BUILD_VAR(Time, ghc::Tag::time);
    GH_BUILD_VAR(DateTime, ghc::Tag::datetime);
    GH_BUILD_VAR(Slider, ghc::Tag::slider);
    GH_BUILD_VAR(Spinner, ghc::Tag::spinner);
    GH_BUILD_VAR(Select, ghc::Tag::select);
    GH_BUILD_VAR(Color, ghc::Tag::color);
    GH_BUILD_VAR(Switch, ghc::Tag::switch_t);
    GH_BUILD_VAR(SwitchIcon, ghc::Tag::switch_i);
    GH_BUILD_VAR(Tabs, ghc::Tag::tabs);
    GH_BUILD_VAR(Confirm, ghc::Tag::confirm);
    GH_BUILD_VAR(Prompt, ghc::Tag::prompt);
    GH_BUILD_VAR_TYPE(Button, gh::Button*, ghc::Tag::button);
    GH_BUILD_VAR_TYPE(Flags, gh::Flags*, ghc::Tag::flags);
    ghc::Widget& Joystick_(GHTREF name = GHTXT(), Pos* pos = nullptr, bool keep = 0, bool exp = 0) {
        return _joy_pad(ghc::Tag::joy, name, pos, keep, exp);
    }
    ghc::Widget& Joystick(Pos* pos = nullptr, bool keep = 0, bool exp = 0) {
        return Joystick_(GHTXT(), pos, keep, exp);
    }
    ghc::Widget& Dpad_(GHTREF name = GHTXT(), Pos* pos = nullptr) {
        return _joy_pad(ghc::Tag::dpad, name, pos);
    }
    ghc::Widget& Dpad(Pos* pos = nullptr) {
        return Dpad_(GHTXT(), pos);
    }
    GH_BUILD_VAL(Title, ghc::Tag::title);
    GH_BUILD_VAL(Label, ghc::Tag::label);
    GH_BUILD_VAL(Text, ghc::Tag::text);
    GH_BUILD_VAL(TextFile, ghc::Tag::text_f);
    GH_BUILD_VAL(Display, ghc::Tag::display);
    GH_BUILD_VAL(Image, ghc::Tag::image);
    GH_BUILD_VAR_TYPE(Log, gh::Log*, ghc::Tag::log);
    GH_BUILD_VAR(LED, ghc::Tag::led);
    GH_BUILD_VAR(Icon, ghc::Tag::icon);
    GH_BUILD_VAR(Gauge, ghc::Tag::gauge);
    GH_BUILD_VAR(GaugeRound, ghc::Tag::gauge_r);
    GH_BUILD_VAR(GaugeLinear, ghc::Tag::gauge_l);
    ghc::Widget& Table_(GHTREF name, GHTREF text, GHTREF width = GHTXT(), GHTREF align = GHTXT()) {
        _varAndType(ghc::Tag::table, name, &text);
        if (width.valid()) widget.param(ghc::Tag::width, width);
        if (align.valid()) widget.param(ghc::Tag::align, align);
        return widget;
    }
    ghc::Widget& Table(GHTREF text, GHTREF width = GHTXT(), GHTREF align = GHTXT()) {
        return Table_(GHTXT(), text, width, align);
    }
    void Stream(uint16_t port = 82) {
        _checkName();
        switch (build.action) {
            case Action::UI:
                if (!widget._enabled) break;
                _beginName();
                _tag(ghc::Tag::stream);
                widget.param(ghc::Tag::port, port);
                break;
            default:
                break;
        }
    }
    ghc::Widget& BeginCanvas(uint16_t width = 400, uint16_t height = 300, Canvas* cv = nullptr, Pos* pos = nullptr) {
        return BeginCanvas_(GHTXT(), width, height, cv, pos);
    }
    ghc::Widget& BeginCanvas_(GHTREF name = GHTXT(), uint16_t width = 400, uint16_t height = 300, Canvas* cv = nullptr, Pos* pos = nullptr) {
        return Canvas_(name, width, height, cv, pos, true);
    }
    ghc::Widget& Canvas(uint16_t width = 400, uint16_t height = 300, Canvas* cv = nullptr, Pos* pos = nullptr) {
        return Canvas_(GHTXT(), width, height, cv, pos);
    }
    ghc::Widget& Canvas_(GHTREF name = GHTXT(), uint16_t width = 400, uint16_t height = 300, gh::Canvas* cv = nullptr, Pos* pos = nullptr, bool begin = false) {
        ghc::AnyPtr data(pos);
        _checkName(name);
        if (cv && build.action != Action::UI) cv->setBuffer(nullptr);
        switch (build.action) {
            case Action::UI:
                if (!widget._enabled) break;
                _beginName(name);
                _tag(ghc::Tag::canvas);
                widget.param(ghc::Tag::width, width);
                widget.param(ghc::Tag::height, height);
                if (pos) widget.param(ghc::Tag::active, 1);
                p->beginArr(ghc::Tag::data);
                if (begin && cv) cv->setBuffer(p);
                else EndCanvas();
                break;
            case Action::Set:
                widget._click = _parseName(name, data);
                if (widget._click) _needs_update = false;
                break;
            default:
                break;
        }
        return widget;
    }
    void EndCanvas() {
        if (build.action == Action::UI) p->endArr();
    }
    ghc::Widget& Menu(GHTREF text) {
        widget._click = 0;
        if (_menu) {
            if (_varAndType(ghc::Tag::menu, F("_menu"), _menu).text(text).click()) {
                refresh();
                _needs_update = false;
            }
        }
        return widget;
    }
    GH_BUILD_VAR(Dummy, ghc::Tag::dummy);
    GH_BUILD_VAL(HTML, ghc::Tag::html);
    GH_BUILD_VAL_NONAME(JS, ghc::Tag::js);
    GH_BUILD_VAL_NONAME(CSS, ghc::Tag::css);
    GH_BUILD_CUSTOM(Custom, ghc::Tag::func);
    void Plugin(GHTREF js, GHTREF css = GHTXT()) {
        _checkName();
        switch (build.action) {
            case Action::UI:
                if (!widget._enabled) break;
                _beginName();
                _tag(ghc::Tag::plugin);
                widget.param(ghc::Tag::js, js);
                widget.param(ghc::Tag::css, css);
                break;
            default:
                break;
        }
    }
   private:
    ghc::Widget& _varAndType(ghc::Tag tag, GHTREF name, const ghc::AnyPtr& data, GHTREF func = GHTXT()) {
        _checkName(name);
        switch (build.action) {
            case Action::UI:
                if (!widget._enabled) break;
                _beginName(name);
                _tag(tag);
                _func(func);
                _uiHelper(name, data);
                break;
            case Action::Get:
                _getHelper(name, data);
                break;
            case Action::Read:
                if (_readName(name) && data.ptr) _writeValue(name, data);
                break;
            case Action::Set:
                _setHelper(name, data);
                break;
            default:
                break;
        }
        return widget;
    }
    ghc::Widget& _joy_pad(ghc::Tag tag, GHTREF name, Pos* pos = nullptr, bool keep = 0, bool exp = 0) {
        ghc::AnyPtr data(pos);
        _checkName(name);
        switch (build.action) {
            case Action::UI:
                if (!widget._enabled) break;
                _beginName(name);
                _tag(tag);
                if (keep) widget.param(ghc::Tag::keep, 1);
                if (exp) widget.param(ghc::Tag::exp, 1);
                break;
            case Action::Set:
                widget._click = _parseName(name, data);
                if (widget._click) {
                    _needs_update = false;
                    if (pos) {
                        pos->x -= 255;
                        pos->y -= 255;
                    }
                }
                break;
            default:
                break;
        }
        return widget;
    }
    ghc::Packet* p;
    uint16_t _index = 0;
    bool _first = true;
    bool _stop = false;
    bool _refresh = false;
    bool _changed = false;
    bool _needs_update = true;
    uint8_t* _menu = nullptr;
    void _tag(ghc::Tag tag) {
        p->addString(ghc::Tag::type, tag);
    }
    void _func(GHTREF func) {
        if (!func.valid()) return;
        p->addString(ghc::Tag::func, func);
    }
    bool _checkFirst() {
        if (_first) return _first = false;
        return 1;
    }
    void _writeValue(GHTREF name, const ghc::AnyPtr& data) {
        switch (data.type) {
            case Type::BTN_T:
            case Type::NULL_T:
                break;
#ifndef GH_NO_PAIRS
            case Type::PAIRS_T: {
                Pair pair = ((Pairs*)data.ptr)->get(name);
                if (!pair.valid()) pair = ((Pairs*)data.ptr)->add(name, "");
                if (pair.valid()) ghc::varToStr(*p, ghc::AnyPtr(&pair));
            } break;
#endif
            default:
                ghc::varToStr(*p, data);
                break;
        }
    }
    void _beginName(GHTREF name = GHTXT()) {
        if (_checkFirst()) p->endObj();
        p->beginObj();
        p->addKey(ghc::Tag::id);
        p->quotes();
        if (name.length()) {
            name.addString(*p);
        } else {
            p->s += F("_n");
            p->s += _index;
        }
        p->quotes();
        p->comma();
    }
    void _checkName(GHTREF name = GHTXT()) {
        if (!name.length()) _index++;
    }
    bool _nameEq(GHTREF name) {
        if (name.valid()) return (name == build.name);
        else return _autoNameEq();
    }
    bool _autoNameEq() {
        return build.name[0] == '_' && build.name[1] == 'n' && ((uint16_t)atoi(build.name.str() + 2) == _index);
    }
    bool _readName(GHTREF name) {
        if (_stop) return false;
        if (name.valid()) {
            if (name == build.name) return _stop = true;
        } else {
            if (_autoNameEq()) return _stop = true;
        }
        return false;
    }
    bool _parseName(GHTREF name, const ghc::AnyPtr& data) {
        if (!_stop && _nameEq(name)) {
            _stop = true;
            if (!data.ptr) return 1;
            if (data.type == Type::PAIRS_T) {
#ifndef GH_NO_PAIRS
                ((Pairs*)data.ptr)->set(name, build.value);
#endif
            } else {
                ghc::strToVar(build.value, data);
            }
            return 1;
        }
        return 0;
    }
    bool _beginContainer(uint16_t width, ghc::Tag tag) {
        if (!p || !_allowContainer()) return true;
        if (_checkFirst()) p->endObj();
        p->beginObj();
        p->addString(ghc::Tag::type, tag);
        p->addInt(ghc::Tag::wwidth, width);
        p->beginArr(ghc::Tag::data);
        _first = true;
        return true;
    }
    void _endContainer() {
        if (!p || !_allowContainer()) return;
        if (_checkFirst()) p->endObj();
        p->endArr();
        _first = false;
    }
    bool _nextContainer(uint16_t width, ghc::Tag tag) {
        if (!p || !_allowContainer()) return true;
        _endContainer();
        _beginContainer(width, tag);
        return true;
    }
    bool _allowContainer() {
        return build.action == Action::UI && widget._enabled;
    }
    void _uiHelper(GHTREF name, const ghc::AnyPtr& data) {
        if (data.ptr) {
            switch (data.type) {
                case Type::NULL_T:
                case Type::BTN_T:
                    break;
#ifndef GH_NO_PAIRS
                case Type::PAIRS_T: {
                    Pair pair = ((Pairs*)data.ptr)->get(name);
                    if (pair.valid()) widget.valueVar(ghc::AnyPtr(&pair));
                } break;
#endif
                default:
                    widget.valueVar(data);
                    break;
            }
        }
    }
    void _getHelper(GHTREF name, const ghc::AnyPtr& data) {
        if (name.valid() && data.ptr) {
            p->addKey(name);
            p->quotes();
            _writeValue(name, data);
            p->quotes();
            p->comma();
        }
    }
    void _setHelper(GHTREF name, const ghc::AnyPtr& data) {
        switch (data.type) {
            case Type::TEXT_T:
                break;
            case Type::BTN_T:
                widget._click = 0;
                if (_parseName(name, data)) {
                    widget._click = (build.value[0] == '2');
                    _needs_update = false;
                }
                break;
            default:
                widget._click = _parseName(name, data);
                if (widget._click) _changed = true;
                break;
        }
    }
};
}