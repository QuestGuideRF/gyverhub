#pragma once
#include <Arduino.h>
#include <StringUtils.h>
#include "client.h"
#include "request_class.h"
#include "types.h"
namespace gh {
class Request {
   public:
    Request(Client& client,
            CMD cmd,
            GHTREF name = GHTXT(),
            GHTREF value = GHTXT()) : client(client),
                                      cmd(cmd),
                                      name(name),
                                      value(value) {}
    Client& client;
    const CMD cmd;
    const GHTXT name;
    const GHTXT value;
};
}