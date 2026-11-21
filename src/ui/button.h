#pragma once
#include <Arduino.h>
namespace gh {
class Button {
   public:
    bool changed() {
        return _changed ? (_changed = 0, 1) : 0;
    }
    bool state() {
        return _state;
    }
    bool clicked() {
        return _clicked ? (_clicked = 0, 1) : 0;
    }
    bool _state = 0;
    bool _changed = 0;
    bool _clicked = 0;
};
}