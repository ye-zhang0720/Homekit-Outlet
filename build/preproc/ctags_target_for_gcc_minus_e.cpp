# 1 "/Users/zhangye/Documents/智能家居/HomekitOutlet/HomekitOutlet.ino"
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


# 19 "/Users/zhangye/Documents/智能家居/HomekitOutlet/HomekitOutlet.ino" 2
# 20 "/Users/zhangye/Documents/智能家居/HomekitOutlet/HomekitOutlet.ino" 2
# 21 "/Users/zhangye/Documents/智能家居/HomekitOutlet/HomekitOutlet.ino" 2
# 22 "/Users/zhangye/Documents/智能家居/HomekitOutlet/HomekitOutlet.ino" 2

# 24 "/Users/zhangye/Documents/智能家居/HomekitOutlet/HomekitOutlet.ino" 2
# 25 "/Users/zhangye/Documents/智能家居/HomekitOutlet/HomekitOutlet.ino" 2

# 27 "/Users/zhangye/Documents/智能家居/HomekitOutlet/HomekitOutlet.ino" 2
# 28 "/Users/zhangye/Documents/智能家居/HomekitOutlet/HomekitOutlet.ino" 2
# 42 "/Users/zhangye/Documents/智能家居/HomekitOutlet/HomekitOutlet.ino"
//按键配置
ButtonDebounce btn(13, 0x02, 0x0);
ButtonHandler btnHandler;

WiFiManager wifiManager;

void blink_led(unsigned char pin, int interval, int count) {
 for (int i = 0; i < count; i++) {
  builtinledSetStatus(16, true);
  delay(interval);
  builtinledSetStatus(16, false);
  delay(interval);
 }
}

void builtinledSetStatus(unsigned char pin, bool on) {
 digitalWrite(pin, on ? 0x0 : 0x1);
}


void __attribute__((section("\".iram.text." "HomekitOutlet.ino" "." "62" "." "133" "\""))) btnInterrupt() {
  btn.update();
}



void setup() {
 Serial.begin(115200);


 pinMode(16, 0x01);
    pinMode(12, 0x01);
    pinMode(14, 0x01);

    blink_led(16,200, 3);

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


 blink_led(12,200, 3);
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
 outlet_in_use.value.bool_value = on; //sync the value


     printf("Switch: %s\n", on ? "ON" : "OFF");


    digitalWrite(14, on ? 0x0 : 0x1);
    digitalWrite(12, on ? 0x0 : 0x1);
}

void outlet_toggle() {
 outlet_in_use.value.bool_value = !outlet_in_use.value.bool_value;
 outlet_in_use.setter(outlet_in_use.value);
 homekit_characteristic_notify(&outlet_in_use, outlet_in_use.value);
    digitalWrite(14, outlet_in_use.value.bool_value ? 0x0 : 0x1);
    digitalWrite(12, outlet_in_use.value.bool_value ? 0x0 : 0x1);
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

            Serial.println("Button Event: SINGLECLICK");

        outlet_toggle();
    } else if (e == BUTTON_EVENT_LONGCLICK) {

            Serial.println("Button Event: LONGCLICK");
            Serial.println("Rebooting...");

        blink_led(16, 200, 3);
        homekit_storage_reset();
        wifiManager.resetSettings();
        blink_led(16, 200, 3);
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
  printf_P(
# 173 "/Users/zhangye/Documents/智能家居/HomekitOutlet/HomekitOutlet.ino" 3
 (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "HomekitOutlet.ino" "." "173" "." "134" "\", \"aSM\", @progbits, 1 #"))) = (
# 173 "/Users/zhangye/Documents/智能家居/HomekitOutlet/HomekitOutlet.ino"
 ">>> [%7d] HomeKit: " "heap: %d, sockets: %d" "\n"
# 173 "/Users/zhangye/Documents/智能家居/HomekitOutlet/HomekitOutlet.ino" 3
 ); &__pstr__[0];})) 
# 173 "/Users/zhangye/Documents/智能家居/HomekitOutlet/HomekitOutlet.ino"
 , millis(), ESP.getFreeHeap(), arduino_homekit_connected_clients_count());;
 }
}
