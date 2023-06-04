#include "wifi_connect.h"

AsyncWebServer server(80);
DNSServer dns;

bool shouldSaveConfig = false;

// callback notifying us of the need to save config
void saveConfigCallback() {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void wifi_init() {
  menu_print(0, 1, "Connecting to WiFi  ");

  AsyncWiFiManager wifiManager(&server, &dns);

  wifiManager.setSaveConfigCallback(saveConfigCallback);

  bool connected = false;
  if (!digitalRead(Button1)) {  // Reset when Button1 is pressed on Boot
    Serial.println("startConfigPortal");
    wifiManager.setTryConnectDuringConfigPortal(false);
    connected = wifiManager.startConfigPortal("ShotBot Config");
  } else {
    connected = wifiManager.autoConnect("ShotBot Config");
  }
  if (!connected) {
    menu_print(0, 0, "####################");
    menu_print(0, 1, "failed to connect \nand hit timeout");
    menu_print(0, 3, "####################");
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    ESP.restart();
    delay(5000);
  }
  Serial.println(wifiManager.infoAsString());
  menu_print(0, 2, WiFi.localIP().toString());

  ArduinoOTA.setHostname("ShotBot");
  ArduinoOTA.begin();
}

void wifi_loop() {
  //
  ArduinoOTA.handle();
}
