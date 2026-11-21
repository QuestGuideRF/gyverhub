#pragma once
#include <Arduino.h>
#include <StringUtils.h>
#include "hub_macro.hpp"
#include "core/packet.h"
enum class CV : uint8_t {
    BUTT,
    ROUND,
    SQUARE,
    PROJECT,
    BEVEL,
    MITER,
    START,
    END,
    CENTER,
    LEFT,
    RIGHT,
    ALPHABETIC,
    TOP,
    HANGING,
    MIDDLE,
    IDEOGRAPHIC,
    BOTTOM,
    SRC_OVER,
    SRC_ATOP,
    SRC_IN,
    SRC_OUT,
    DST_OVER,
    DST_ATOP,
    DST_IN,
    DST_OUT,
    LIGHTER,
    COPY,
    XOR,
    TXT_TOP,
    TXT_BOTTOM,
    TXT_CENTER,
    TXT_BASELINE,
    CORNER,
    CORNERS,
    RADIUS,
};
namespace gh {
class Canvas {
   public:
    void setBuffer(ghc::Packet* pp) {
        p = pp;
    }
    Canvas& custom(GHTREF text) {
        if (!p) return *this;
        _checkFirst();
        p->quotes();
        p->addTextRaw(text);
        p->quotes();
        return *this;
    }
    Canvas& clear() {
        clearRect(0, 0, -1, -1);
        beginPath();
        return *this;
    }
    Canvas& background() {
        fillRect(0, 0, -1, -1);
        return *this;
    }
    Canvas& background(uint32_t hex, uint8_t a = 255) {
        fillStyle(hex, a);
        background();
        return *this;
    }
    Canvas& fill(uint32_t hex, uint8_t a = 255) {
        fillStyle(hex, a);
        _fillF = 1;
        return *this;
    }
    Canvas& noFill() {
        _fillF = 0;
        return *this;
    }
    Canvas& stroke(uint32_t hex, uint8_t a = 255) {
        strokeStyle(hex, a);
        _strokeF = 1;
        return *this;
    }
    Canvas& noStroke() {
        _strokeF = 0;
        return *this;
    }
    Canvas& strokeWeight(int v) {
        lineWidth(v);
        return *this;
    }
    Canvas& strokeJoin(CV v) {
        lineJoin(v);
        return *this;
    }
    Canvas& strokeCap(CV v) {
        lineCap(v);
        return *this;
    }
    Canvas& circle(int x, int y, int r) {
        beginPath();
        switch (_eMode) {
            case CV::CORNER:
                arc(x + r, y + r, r);
                break;
            default:
                arc(x, y, r);
                break;
        }
        if (_strokeF) stroke();
        if (_fillF) fill();
        return *this;
    }
    Canvas& line(int x1, int y1, int x2, int y2) {
        beginPath();
        moveTo(x1, y1);
        lineTo(x2, y2);
        stroke();
        return *this;
    }
    Canvas& point(int x, int y) {
        beginPath();
        fillRect(x, y, 1, 1);
        return *this;
    }
    Canvas& quadrangle(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) {
        beginPath();
        moveTo(x1, y1);
        lineTo(x2, y2);
        lineTo(x3, y3);
        lineTo(x4, y4);
        closePath();
        if (_strokeF) stroke();
        if (_fillF) fill();
        return *this;
    }
    Canvas& triangle(int x1, int y1, int x2, int y2, int x3, int y3) {
        beginPath();
        moveTo(x1, y1);
        lineTo(x2, y2);
        lineTo(x3, y3);
        closePath();
        if (_strokeF) stroke();
        if (_fillF) fill();
        return *this;
    }
    Canvas& rect(int x, int y, int w, int h, int tl = -1, int tr = -1, int br = 0, int bl = 0) {
        beginPath();
        int X = x, Y = y, W = w, H = h;
        switch (_rMode) {
            case CV::CORNER:
                break;
            case CV::CORNERS:
                W = w - x;
                H = h - y;
                break;
            case CV::CENTER:
                X = x - w / 2;
                Y = y - h / 2;
                break;
            case CV::RADIUS:
                X = x - w;
                Y = y - h;
                W = w * 2;
                H = h * 2;
                break;
            default:
                break;
        }
        if (tl < 0) drawRect(X, Y, W, H);
        else if (tr < 0) roundRect(X, Y, W, H, tl);
        else roundRect(X, Y, W, H, tl, tr, br, bl);
        if (_strokeF) stroke();
        if (_fillF) fill();
        return *this;
    }
    Canvas& square(int x, int y, int w) {
        rect(x, y, w, w);
        return *this;
    }
    Canvas& ellipseMode(CV mode) {
        _eMode = mode;
        return *this;
    }
    Canvas& rectMode(CV mode) {
        _rMode = mode;
        return *this;
    }
    Canvas& textFont(const char* name) {
        _fname = name;
        _font();
        return *this;
    }
    Canvas& textSize(int size) {
        _fsize = size;
        _font();
        return *this;
    }
    Canvas& text(const String& text, int x, int y, int w = 0) {
        if (_strokeF) strokeText(text, x, y, w);
        if (_fillF) fillText(text, x, y, w);
        return *this;
    }
    Canvas& textAlign(CV h, CV v) {
        textAlign(h);
        textBaseline(v);
        return *this;
    }
    Canvas& push() {
        save();
        return *this;
    }
    Canvas& pop() {
        restore();
        return *this;
    }
    Canvas& fillStyle(uint32_t hex, uint8_t a = 255) {
        if (!p) return *this;
        _cmd(0);
        p->colon();
        _color(hex, a);
        p->quotes();
        return *this;
    }
    Canvas& strokeStyle(uint32_t hex, uint8_t a = 255) {
        if (!p) return *this;
        _cmd(1);
        p->colon();
        _color(hex, a);
        p->quotes();
        return *this;
    }
    Canvas& shadowColor(uint32_t hex, uint8_t a = 255) {
        if (!p) return *this;
        _cmd(2);
        p->colon();
        _color(hex, a);
        p->quotes();
        return *this;
    }
    Canvas& shadowBlur(int v) {
        if (!p) return *this;
        _cmd(3);
        _addP(v);
        return *this;
    }
    Canvas& shadowOffsetX(int v) {
        if (!p) return *this;
        _cmd(4);
        _addP(v);
        return *this;
    }
    Canvas& shadowOffsetY(int v) {
        if (!p) return *this;
        _cmd(5);
        _addP(v);
        return *this;
    }
    Canvas& lineCap(CV v) {
        if (!p) return *this;
        _cmd(11);
        _addP((uint8_t)v);
        return *this;
    }
    Canvas& lineJoin(CV v) {
        if (!p) return *this;
        _cmd(12);
        _addP((uint8_t)v);
        return *this;
    }
    Canvas& lineWidth(int v) {
        if (!p) return *this;
        _cmd(6);
        _addP(v);
        return *this;
    }
    Canvas& miterLimit(int v) {
        if (!p) return *this;
        _cmd(7);
        _addP(v);
        return *this;
    }
    Canvas& font(GHTREF v) {
        if (!p) return *this;
        _cmd(8);
        p->colon();
        p->addTextRaw(v);
        p->quotes();
        return *this;
    }
    Canvas& textAlign(CV v) {
        if (!p) return *this;
        _cmd(9);
        _addP((uint8_t)v);
        return *this;
    }
    Canvas& textBaseline(CV v) {
        if (!p) return *this;
        _cmd(10);
        _addP((uint8_t)v);
        return *this;
    }
    Canvas& globalAlpha(float v) {
        if (!p) return *this;
        _cmd(14);
        _addP(v);
        return *this;
    }
    Canvas& globalCompositeOperation(CV v) {
        if (!p) return *this;
        _cmd(13);
        _addP((uint8_t)v);
        return *this;
    }
    Canvas& drawRect(int x, int y, int w, int h) {
        if (!p) return *this;
        _cmd(17);
        p->colon();
        _params(4, x, y, w, h);
        p->quotes();
        return *this;
    }
    Canvas& roundRect(int x, int y, int w, int h, int tl = 0, int tr = -1, int br = -1, int bl = -1) {
        if (!p) return *this;
        _cmd(31);
        p->colon();
        if (tr < 0) _params(5, x, y, w, h, tl);
        else if (br < 0) _params(6, x, y, w, h, tl, tr);
        else _params(8, x, y, w, h, tl, tr, br, bl);
        p->quotes();
        return *this;
    }
    Canvas& fillRect(int x, int y, int w, int h) {
        if (!p) return *this;
        _cmd(18);
        p->colon();
        _params(4, x, y, w, h);
        p->quotes();
        return *this;
    }
    Canvas& strokeRect(int x, int y, int w, int h) {
        if (!p) return *this;
        _cmd(19);
        p->colon();
        _params(4, x, y, w, h);
        p->quotes();
        return *this;
    }
    Canvas& clearRect(int x, int y, int w, int h) {
        if (!p) return *this;
        _cmd(20);
        p->colon();
        _params(4, x, y, w, h);
        p->quotes();
        return *this;
    }
    Canvas& fill() {
        if (!p) return *this;
        _cmd(32);
        p->quotes();
        return *this;
    }
    Canvas& stroke() {
        if (!p) return *this;
        _cmd(33);
        p->quotes();
        return *this;
    }
    Canvas& beginPath() {
        if (!p) return *this;
        _cmd(34);
        p->quotes();
        return *this;
    }
    Canvas& moveTo(int x, int y) {
        if (!p) return *this;
        _cmd(21);
        p->colon();
        _params(2, x, y);
        p->quotes();
        return *this;
    }
    Canvas& closePath() {
        if (!p) return *this;
        _cmd(35);
        p->quotes();
        return *this;
    }
    Canvas& lineTo(int x, int y) {
        if (!p) return *this;
        _cmd(22);
        p->colon();
        _params(2, x, y);
        p->quotes();
        return *this;
    }
    Canvas& clip() {
        if (!p) return *this;
        _cmd(36);
        p->quotes();
        return *this;
    }
    Canvas& quadraticCurveTo(int cpx, int cpy, int x, int y) {
        if (!p) return *this;
        _cmd(23);
        p->colon();
        _params(4, cpx, cpy, x, y);
        p->quotes();
        return *this;
    }
    Canvas& bezierCurveTo(int cp1x, int cp1y, int cp2x, int cp2y, int x, int y) {
        if (!p) return *this;
        _cmd(24);
        p->colon();
        _params(6, cp1x, cp1y, cp2x, cp2y, x, y);
        p->quotes();
        return *this;
    }
    Canvas& arc(int x, int y, int r, float sa = 0, float ea = TWO_PI, bool ccw = 0) {
        if (!p) return *this;
        _cmd(27);
        p->colon();
        _params(3, x, y, r);
        p->comma();
        p->s += sa;
        p->comma();
        p->s += ea;
        p->s += ccw;
        p->quotes();
        return *this;
    }
    Canvas& arcTo(int x1, int y1, int x2, int y2, int r) {
        if (!p) return *this;
        _cmd(26);
        p->colon();
        _params(5, x1, y1, x2, y2, r);
        p->quotes();
    }
    Canvas& scale(int sw, int sh) {
        if (!p) return *this;
        _cmd(15);
        p->colon();
        _params(2, sw, sh);
        p->quotes();
        return *this;
    }
    Canvas& rotate(float v) {
        if (!p) return *this;
        _cmd(16);
        _addP(v);
        return *this;
    }
    Canvas& translate(int x, int y) {
        if (!p) return *this;
        _cmd(25);
        p->colon();
        _params(2, x, y);
        p->quotes();
        return *this;
    }
    Canvas& fillText(GHTREF text, int x, int y, int w = 0) {
        if (!p) return *this;
        _cmd(28);
        p->colon();
        p->quotes();
        p->addStringEsc(text);
        p->quotes();
        p->comma();
        _params(3, x, y, w);
        p->quotes();
        return *this;
    }
    Canvas& strokeText(GHTREF text, int x, int y, int w = 0) {
        if (!p) return *this;
        _cmd(29);
        p->colon();
        p->quotes();
        p->addStringEsc(text);
        p->quotes();
        p->comma();
        _params(3, x, y, w);
        p->quotes();
        return *this;
    }
    Canvas& drawImage(GHTREF img, int x, int y) {
        if (!p) return *this;
        _cmd(30);
        p->colon();
        p->addText(img);
        p->comma();
        _params(2, x, y);
        p->quotes();
        return *this;
    }
    Canvas& drawImage(GHTREF img, int x, int y, int w) {
        if (!p) return *this;
        _cmd(30);
        p->colon();
        p->addText(img);
        p->comma();
        _params(3, x, y, w);
        p->quotes();
        return *this;
    }
    Canvas& drawImage(GHTREF img, int x, int y, int w, int h) {
        if (!p) return *this;
        _cmd(30);
        p->colon();
        p->addText(img);
        p->comma();
        _params(4, x, y, w, h);
        p->quotes();
        return *this;
    }
    Canvas& drawImage(GHTREF img, int sx, int sy, int sw, int sh, int x, int y, int w, int h) {
        if (!p) return *this;
        _cmd(30);
        p->colon();
        p->addText(img);
        p->comma();
        _params(8, sx, sy, sw, sh, x, y, w, h);
        p->quotes();
        return *this;
    }
    Canvas& save() {
        if (!p) return *this;
        _cmd(37);
        p->quotes();
        return *this;
    }
    Canvas& restore() {
        if (!p) return *this;
        _cmd(38);
        p->quotes();
        return *this;
    }
   private:
    void _checkFirst() {
        if (!p) return;
        if (_first) _first = 0;
        else p->comma();
    }
    void _cmd(int cmd) {
        if (!p) return;
        _checkFirst();
        p->quotes();
        p->s += cmd;
    }
    void _params(int num, ...) {
        va_list valist;
        va_start(valist, num);
        for (int i = 0; i < num; i++) {
            p->s += va_arg(valist, int);
            if (i < num - 1) p->comma();
        }
        va_end(valist);
    }
    void _color(uint32_t hex, uint8_t a = 255) {
        p->s += ((uint32_t)hex << 8) | a;
    }
    void _font() {
        if (!p) return;
        _cmd(10);
        p->colon();
        p->s += '\'';
        p->s += _fsize;
        p->s += F("px ");
        p->s += _fname;
        p->s += '\'';
        p->quotes();
    }
    void _addP(int val) {
        p->colon();
        p->s += val;
        p->quotes();
    }
    ghc::Packet* p = nullptr;
    bool _first = 1;
    bool _strokeF = 1;
    bool _fillF = 1;
    const char* _fname = "Arial";
    uint16_t _fsize = 20;
    CV _eMode = CV::RADIUS;
    CV _rMode = CV::CORNER;
};
}