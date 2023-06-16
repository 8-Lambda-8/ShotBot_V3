#ifndef WIFI_CONNECT_H /* include guards */
#define WIFI_CONNECT_H

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESPAsyncWiFiManager.h>
#include <OSCMessage.h>
#include <WiFiUdp.h>

#include "menu.h"
#include "pin_config.h"

void wifi_init();
void wifi_loop();

#endif /* WIFI_CONNECT_H */