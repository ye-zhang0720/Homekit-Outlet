/*
 * outlet.ino
 *
 * This accessory contains tow builtin-leds, a button and a relay on ESP M4
 * Setup code: 111-11-111
 * The Red Led is linked to GPIO 16
 * The Blue Led is linked to GPIO 12
 * The Button is linked to GPIO 13
 * The Relay is linked to GPIO 14
 * 
 * 
 *  Created on: 2021-09-19
 *      Author: ye zhang
 *      
 */


#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <arduino_homekit_server.h>

#include "ButtonDebounce.h"
#include "ButtonHandler.h"

#define __DEBUG
#define PL(s) Serial.println(s)
#define P(s) Serial.print(s)



#define PIN_RED_LED 16
#define PIN_BLUE_LED 12
#define PIN_BUTTON 13
#define PIN_Switch  14



//按键配置
ButtonDebounce btn(PIN_BUTTON, INPUT_PULLUP, LOW);
ButtonHandler btnHandler;

WiFiManager wifiManager;

void blink_led(unsigned char pin, int interval, int count) {
	for (int i = 0; i < count; i++) {
		builtinledSetStatus(PIN_RED_LED, true);
		delay(interval);
		builtinledSetStatus(PIN_RED_LED, false);
		delay(interval);
	}
}

void builtinledSetStatus(unsigned char pin, bool on) {
	digitalWrite(pin, on ? LOW : HIGH);
}


void IRAM_ATTR btnInterrupt() {
  btn.update();
}



void setup() {
	Serial.begin(115200);


	pinMode(PIN_RED_LED, OUTPUT);
    pinMode(PIN_BLUE_LED, OUTPUT);
    pinMode(PIN_Switch, OUTPUT);

    blink_led(PIN_RED_LED,200, 3);

    wifiManager.setBreakAfterConfig(true);
    if (!wifiManager.autoConnect("AutoConnectAP", "password")) {
      Serial.println("failed to connect, we should reset as see if it connects");
      delay(3000);
      ESP.reset();
      delay(5000);
    }

    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
    Serial.println("local ip");
    Serial.println(WiFi.localIP());

	homekit_setup();


	blink_led(PIN_BLUE_LED,200, 3);
}

void loop() {
	homekit_loop();
	delay(5);
}


//==============================
// Homekit setup and loop
//==============================

extern "C" homekit_server_config_t config;
extern "C" homekit_characteristic_t outlet_in_use;
//extern "C" void outlet_toggle();




void outlet_in_use_setter(const homekit_value_t value) {
	bool on = value.bool_value;
	outlet_in_use.value.bool_value = on;	//sync the value
    
    #ifdef __DEBUG
	    printf("Switch: %s\n", on ? "ON" : "OFF");
	#endif

    digitalWrite(PIN_Switch, on ? LOW : HIGH);
    digitalWrite(PIN_BLUE_LED, on ? LOW : HIGH);
}

void outlet_toggle() {
	outlet_in_use.value.bool_value = !outlet_in_use.value.bool_value;
	outlet_in_use.setter(outlet_in_use.value);
	homekit_characteristic_notify(&outlet_in_use, outlet_in_use.value);
    digitalWrite(PIN_Switch, outlet_in_use.value.bool_value ? LOW : HIGH);
    digitalWrite(PIN_BLUE_LED, outlet_in_use.value.bool_value ? LOW : HIGH);
}


void homekit_setup() {
	
	outlet_in_use.setter = outlet_in_use_setter;

	arduino_homekit_setup(&config);
    
    btn.setCallback(std::bind(&ButtonHandler::handleChange, &btnHandler,
     std::placeholders::_1));
    btn.setInterrupt(btnInterrupt);
    btnHandler.setIsDownFunction(std::bind(&ButtonDebounce::checkIsDown, &btn));
    btnHandler.setCallback([](button_event e) {
    if (e == BUTTON_EVENT_SINGLECLICK) {
        #ifdef __DEBUG
            Serial.println("Button Event: SINGLECLICK");
        #endif
        outlet_toggle();
    } else if (e == BUTTON_EVENT_LONGCLICK) {
        #ifdef __DEBUG
            Serial.println("Button Event: LONGCLICK");
            Serial.println("Rebooting...");
        #endif
        blink_led(PIN_RED_LED, 200, 3);
        homekit_storage_reset();
        wifiManager.resetSettings();
        blink_led(PIN_RED_LED, 200, 3);
        ESP.reset(); // or system_restart();
    }
  });
}

static unsigned long next_heap_millis = 0;

void homekit_loop() {
    btnHandler.loop();
	arduino_homekit_loop();
    const unsigned long t = millis();
	if (t > next_heap_millis) {
        next_heap_millis = t + 5 * 1000;
		INFO("heap: %d, sockets: %d", ESP.getFreeHeap(), arduino_homekit_connected_clients_count());
	}
}
