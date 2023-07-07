#include "wifi_connect.h"

AsyncWebServer server(80);
DNSServer dns;
WiFiUDP Udp;

const unsigned int rxPort = 8001;

bool shouldSaveConfig = false;

extern uint8_t state;
extern uint8_t selectedDrink;
extern uint8_t selectedCount;
extern uint8_t selectedML;
extern uint8_t animation;
extern uint8_t animationHue;

// callback notifying us of the need to save config
void saveConfigCallback() {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void wifi_init() {
  menu_print(0, 1, "Connecting to WiFi  ");

  AsyncWiFiManager wifiManager(&server, &dns);

  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.setAPCallback(configModeCallback);

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

  menu_print(0, 2, WiFi.localIP().toString());

  ArduinoOTA.setHostname("ShotBot");
  ArduinoOTA.onStart(menu_OTA_Start);
  ArduinoOTA.onProgress(menu_OTA_Progress);
  ArduinoOTA.begin();

  // OSC:
  Udp.begin(rxPort);
}

void routeFill(OSCMessage& msg) { state = 10; }
void routeDrink(OSCMessage& msg) {
  uint8_t drink = msg.getInt(0);
  if (drink < 4) selectedDrink = drink;
}
void routeCount(OSCMessage& msg) {
  uint8_t cnt = msg.getInt(0);
  if (cnt < 24) selectedCount = cnt;
}
void routeML(OSCMessage& msg) {
  uint8_t ml = msg.getInt(0);
  if (ml < 40) selectedML = ml;
}
void routeAnimation(OSCMessage& msg) {
  //
  animation = msg.getInt(0);
}
void routeAnimationHue(OSCMessage& msg) {
  // convert hue degrees to byte
  animationHue = (msg.getInt(0) / 360) * 255;
}
void routeBrightness(OSCMessage& msg) {
  if (!(msg.getInt(0) > 0 && msg.getInt(0) < 256)) return;
  setBrightness(msg.getInt(0));
}

uint16_t size = 0;

void wifi_loop() {
  if (state != 0) return;
  ArduinoOTA.handle();

  OSCMessage msg;
  size = Udp.parsePacket();
  if (size > 0) {
    while (size--) msg.fill(Udp.read());
    if (msg.hasError()) return;
    msg.dispatch("/ShotBot/fill", routeFill);
    msg.dispatch("/ShotBot/drink", routeDrink);
    msg.dispatch("/ShotBot/count", routeCount);
    msg.dispatch("/ShotBot/ml", routeML);
    msg.dispatch("/ShotBot/animation", routeAnimation);
    msg.dispatch("/ShotBot/animation/hue", routeAnimationHue);
    msg.dispatch("/ShotBot/animation/brightness", routeBrightness);
  }
}
