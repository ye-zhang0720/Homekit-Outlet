# 1 "/Users/zhangye/Documents/智能开关/homekit/outlet/outlet.ino"
/*
 * outlet.ino
 *
 * This accessory contains a builtin-led on ESP8266
 * Setup code: 111-11-111
 * The Flash-Button(D3, GPIO0) on NodeMCU:
 *
 *  Created on: 2020-02-08
 *      Author: Mixiaoxiao (Wang Bin)
 *  Edited on: 2020-03-01
 *      Edited by: euler271 (Jonas Linn)
 */

# 15 "/Users/zhangye/Documents/智能开关/homekit/outlet/outlet.ino" 2
# 16 "/Users/zhangye/Documents/智能开关/homekit/outlet/outlet.ino" 2

# 18 "/Users/zhangye/Documents/智能开关/homekit/outlet/outlet.ino" 2




//D0 16 //led
//D3  0 //flash button
//D4  2 //led





const char *ssid = "your-ssid";
const char *password = "your-password";

void blink_led(int interval, int count) {
 for (int i = 0; i < count; i++) {
  builtinledSetStatus(true);
  delay(interval);
  builtinledSetStatus(false);
  delay(interval);
 }
}

void setup() {
 Serial.begin(115200);
 Serial.setRxBufferSize(32);
 Serial.setDebugOutput(false);

 pinMode(16/*D0*/, 0x01);
 WiFi.mode(WIFI_STA);
 WiFi.persistent(false);
 WiFi.disconnect(false);
 WiFi.setAutoReconnect(true);
 WiFi.begin(ssid, password);

 printf("\n");
 printf("SketchSize: %d B\n", ESP.getSketchSize());
 printf("FreeSketchSpace: %d B\n", ESP.getFreeSketchSpace());
 printf("FlashChipSize: %d B\n", ESP.getFlashChipSize());
 printf("FlashChipRealSize: %d B\n", ESP.getFlashChipRealSize());
 printf("FlashChipSpeed: %d\n", ESP.getFlashChipSpeed());
 printf("SdkVersion: %s\n", ESP.getSdkVersion());
 printf("FullVersion: %s\n", ESP.getFullVersion().c_str());
 printf("CpuFreq: %dMHz\n", ESP.getCpuFreqMHz());
 printf("FreeHeap: %d B\n", ESP.getFreeHeap());
 printf("ResetInfo: %s\n", ESP.getResetInfo().c_str());
 printf("ResetReason: %s\n", ESP.getResetReason().c_str());
 ;
 homekit_setup();
 ;
 blink_led(200, 3);
}

void loop() {
 homekit_loop();
 delay(5);
}

void builtinledSetStatus(bool on) {
 digitalWrite(16/*D0*/, on ? 0x0 : 0x1);
}

//==============================
// Homekit setup and loop
//==============================

extern "C" homekit_server_config_t config;
extern "C" homekit_characteristic_t outlet_in_use;


void outlet_in_use_setter(const homekit_value_t value) {
 bool on = value.bool_value;
 outlet_in_use.value.bool_value = on; //sync the value
 printf("Switch: %s\n", on ? "ON" : "OFF");
 digitalWrite(14/*D4*/, on ? 0x0 : 0x1);
}




void homekit_setup() {
 pinMode(14/*D4*/, 0x01);

 outlet_in_use.setter = outlet_in_use_setter;

 arduino_homekit_setup(&config);

}

static unsigned long next_heap_millis = 0;

void homekit_loop() {
 arduino_homekit_loop();
    const unsigned long t = millis();
 if (t > next_heap_millis) {
        next_heap_millis = t + 5 * 1000;
  printf_P(
# 115 "/Users/zhangye/Documents/智能开关/homekit/outlet/outlet.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "outlet.ino" "." "115" "." "9" "\", \"aSM\", @progbits, 1 #"))) = (
# 115 "/Users/zhangye/Documents/智能开关/homekit/outlet/outlet.ino"
 ">>> [%7d] HomeKit: " "heap: %d, sockets: %d" "\n"
# 115 "/Users/zhangye/Documents/智能开关/homekit/outlet/outlet.ino" 3
 ); &__pstr__[0];})) 
# 115 "/Users/zhangye/Documents/智能开关/homekit/outlet/outlet.ino"
 , millis(), ESP.getFreeHeap(), arduino_homekit_connected_clients_count());;
 }
}
