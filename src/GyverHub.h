#ifndef _GyverHub_h
#define _GyverHub_h
#define GH_LIB_VERSION "v0.1b"
#define GH_API_VERSION 1
#ifndef GH_REBOOT_DELAY
#define GH_REBOOT_DELAY 1500
#endif
#ifndef GH_BRIDGE_AMOUNT
#define GH_BRIDGE_AMOUNT 5
#endif
#ifndef GH_FETCH_CHUNK_SIZE
#define GH_FETCH_CHUNK_SIZE 512
#endif
#ifndef GH_UPL_CHUNK_SIZE
#define GH_UPL_CHUNK_SIZE 512
#endif
#ifndef GH_FS_DEPTH
#define GH_FS_DEPTH 5
#endif
#ifndef GH_HTTP_PORT
#define GH_HTTP_PORT 80
#endif
#ifndef GH_CACHE_PRD
#define GH_CACHE_PRD "максимальный возраст=604800"
#endif
#ifndef GH_WS_PORT
#define GH_WS_PORT 81
#endif
#ifndef GH_MQTT_RECONNECT
#define GH_MQTT_RECONNECT 10000
#endif
#ifndef GH_PUBLIC_PATH
#define GH_PUBLIC_PATH "/www"
#endif
#include "core/hub.h"
#include "hub_macro.hpp"
#include "ui/build_helper.h"
#include "ui/button.h"
#include "ui/canvas.h"
#include "ui/canvas_upd.h"
#include "ui/color.h"
#include "ui/csv.h"
#include "ui/csv_file.h"
#include "ui/flags.h"
#include "ui/log.h"
#include "ui/timer.h"
#include "ui/update.h"
namespace GH = gh;
#endif