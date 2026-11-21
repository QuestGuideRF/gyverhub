#pragma once
#include <Arduino.h>
#include <StringUtils.h>
#include "client.h"
#include "hub_macro.hpp"
#include "types.h"
#include "ui/color.h"
#include "ui/flags.h"
#include "ui/pos.h"
namespace gh {
struct Build {
    Build(Action action,
          Client& client,
          GHTREF name = GHTXT(),
          GHTREF value = GHTXT()) : action(action),
                                    client(client),
                                    name(name),
                                    value(value) {}
    const Action action;
    Client& client;
    const GHTXT name;
    const GHTXT value;
    bool isSet() {
        return action == Action::Set;
    }
    bool isUI() {
        return action == Action::UI;
    }
    Color valueColor() {
        return Color(value.toInt32(), HEX);
    }
    Flags valueFlags() {
        return Flags(value.toInt16());
    }
    Pos valuePos() {
        uint32_t xy = value.toInt32();
        return Pos(xy >> 16, xy & 0xffff, true);
    }
};
}