#line 1 "/Users/zhangye/Documents/智能开关/homekit/outlet/outlet.ino"
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

#include <Arduino.h>
#include <ESP8266WiFi.h>

#include <arduino_homekit_server.h>

#define PL(s) Serial.println(s)
#define P(s) Serial.print(s)

//D0 16 //led
//D3  0 //flash button
//D4  2 //led

#define PIN_LED 16//D0
#define PIN_Switch  14//D4


const char *ssid = "your-ssid";
const char *password = "your-password";

#line 33 "/Users/zhangye/Documents/智能开关/homekit/outlet/outlet.ino"
void blink_led(int interval, int count);
#line 42 "/Users/zhangye/Documents/智能开关/homekit/outlet/outlet.ino"
void setup();
#line 72 "/Users/zhangye/Documents/智能开关/homekit/outlet/outlet.ino"
void loop();
#line 77 "/Users/zhangye/Documents/智能开关/homekit/outlet/outlet.ino"
void builtinledSetStatus(bool on);
#line 89 "/Users/zhangye/Documents/智能开关/homekit/outlet/outlet.ino"
void outlet_in_use_setter(const homekit_value_t value);
#line 99 "/Users/zhangye/Documents/智能开关/homekit/outlet/outlet.ino"
void homekit_setup();
#line 110 "/Users/zhangye/Documents/智能开关/homekit/outlet/outlet.ino"
void homekit_loop();
#line 33 "/Users/zhangye/Documents/智能开关/homekit/outlet/outlet.ino"
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

	pinMode(PIN_LED, OUTPUT);
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
	DEBUG_HEAP();
	homekit_setup();
	DEBUG_HEAP();
	blink_led(200, 3);
}

void loop() {
	homekit_loop();
	delay(5);
}

void builtinledSetStatus(bool on) {
	digitalWrite(PIN_LED, on ? LOW : HIGH);
}

//==============================
// Homekit setup and loop
//==============================

extern "C" homekit_server_config_t config;
extern "C" homekit_characteristic_t outlet_in_use;


void outlet_in_use_setter(const homekit_value_t value) {
	bool on = value.bool_value;
	outlet_in_use.value.bool_value = on;	//sync the value
	printf("Switch: %s\n", on ? "ON" : "OFF");
	digitalWrite(PIN_Switch, on ? LOW : HIGH);
}




void homekit_setup() {
	pinMode(PIN_Switch, OUTPUT);
	
	outlet_in_use.setter = outlet_in_use_setter;

	arduino_homekit_setup(&config);
 
}

static unsigned long next_heap_millis = 0;

void homekit_loop() {
	arduino_homekit_loop();
    const unsigned long t = millis();
	if (t > next_heap_millis) {
        next_heap_millis = t + 5 * 1000;
		INFO("heap: %d, sockets: %d", ESP.getFreeHeap(), arduino_homekit_connected_clients_count());
	}
}

