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
#include <FS.h> //this needs to be first, or it all crashes and burns...
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <arduino_homekit_server.h>
#include "ButtonDebounce.h"
#include "ButtonHandler.h"
#include <Ticker.h>

//#define __DEBUG
#define PL(s) Serial.println(s)
#define P(s) Serial.print(s)
#define SIMPLE_INFO(fmt, ...)   printf_P(PSTR(fmt "\n") , ##__VA_ARGS__);
#define PIN_RED_LED 16
#define PIN_BLUE_LED 12
#define PIN_BUTTON 13
#define PIN_Switch 14

//按键配置
ButtonDebounce btn(PIN_BUTTON, INPUT_PULLUP, LOW);
ButtonHandler btnHandler;

WiFiManager wifiManager;
Ticker ticker;

void tick()
{
  // toggle state
  int state = digitalRead(PIN_RED_LED); // get the current state of GPIO1 pin
  digitalWrite(PIN_RED_LED, !state);    // set pin to the opposite state
}

// gets called when WiFiManager enters configuration mode
void configModeCallback(WiFiManager *myWiFiManager)
{
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  // if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  // entered config mode, make led toggle faster
  ticker.attach(0.2, tick);
}

// led闪烁
void blink_led(unsigned char pin, int interval, int count)
{
  for (int i = 0; i < count; i++)
  {
    builtinledSetStatus(pin, true);
    delay(interval);
    builtinledSetStatus(pin, false);
    delay(interval);
  }
}

// led状态改变
void builtinledSetStatus(unsigned char pin, bool on)
{
  digitalWrite(pin, on ? HIGH : LOW);
}

void IRAM_ATTR btnInterrupt()
{
  btn.update();
}

void setup()
{

  Serial.begin(115200);

  pinMode(PIN_RED_LED, OUTPUT);
  pinMode(PIN_BLUE_LED, OUTPUT);
  pinMode(PIN_Switch, OUTPUT);
  digitalWrite(PIN_Switch, LOW);
  digitalWrite(PIN_RED_LED, LOW);
  digitalWrite(PIN_BLUE_LED, LOW);
  wifiManager.setBreakAfterConfig(true);
  wifiManager.setTimeout(120);

  ticker.attach(0.6, tick);
  btn.setCallback(std::bind(&ButtonHandler::handleChange, &btnHandler,
                            std::placeholders::_1));
  btn.setInterrupt(btnInterrupt);
  btnHandler.setIsDownFunction(std::bind(&ButtonDebounce::checkIsDown, &btn));
  btnHandler.setCallback([](button_event e)
                           {
                               if (e == BUTTON_EVENT_SINGLECLICK)
                               {
                                    #ifdef __DEBUG
                                        Serial.println("Button Event: SINGLECLICK");
                                    #endif
                                   outlet_toggle();
                               }
                               else if (e == BUTTON_EVENT_LONGCLICK)
                               {
                                    #ifdef __DEBUG
                                    Serial.println("Button Event: LONGCLICK");
                                    Serial.println("Rebooting...");
                                    #endif

                                    builtinledSetStatus(PIN_RED_LED, true);
                                    
                                    homekit_storage_reset();
                                    wifiManager.setTimeout(120);
                                    SPIFFS.format();
                                    WiFi.disconnect();
                                    wifiManager.setTimeout(60);

                                    wifiManager.startConfigPortal("Homekit_Outlet", "");

                                   blink_led(PIN_BLUE_LED, 200, 3);
                                   
                                   ESP.reset(); // or system_restart();
                               }
                           });


  // wifiManager.resetSettings();
  // set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  // fetches ssid and pass and tries to connect
  // if it does not connect it starts an access point with the specified name
  // here  "AutoConnectAP"
  // and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("Homekit_OutLet"))
  {
    Serial.println("failed to connect and hit timeout");
    // reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }

  // if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  ticker.detach();
  // keep LED off
  digitalWrite(PIN_RED_LED, LOW);

  homekit_setup();
}

void loop()
{
  homekit_loop();
  delay(5);
}

//==============================
// Homekit setup and loop
//==============================

extern "C" homekit_server_config_t config;
extern "C" homekit_characteristic_t outlet_in_use;
extern "C" homekit_characteristic_t name;
extern "C" homekit_characteristic_t serial_number;

#define ChineseWordNumber 4
#define EnglishWordNumber 0
char name_generation[EnglishWordNumber + ChineseWordNumber * 3 + 13] = "智能插座";
char categoryNo = 5;
uint8_t MAC_array_STA[6];
char SN[13];
char Setup_ID[5];
char Setup_code[11];

void GenerateSerialNumber(char *_SN)
{
  /*
   *序列号定义
   *SN_   _ _           _               _         _            _          _        _       _
   *    分类编号 (mac[0]+mac[5])%10  mac[1]对应字母 mac[2]%10 mac[3]%10  mac[4]%10  随机数  随机字母
   */
  char words[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"; // 36个字符

  _SN[0] = 'S';
  _SN[1] = 'N';
  _SN[2] = '_';
  _SN[3] = (char)(categoryNo / 10) % 10 + 48;
  _SN[4] = categoryNo % 10 + 48;
  _SN[5] = (MAC_array_STA[0] + MAC_array_STA[5]) % 10 + 48;
  _SN[6] = words[MAC_array_STA[1] % 36];
  _SN[7] = MAC_array_STA[2] % 10 + 48;
  _SN[8] = words[(MAC_array_STA[3] * MAC_array_STA[2] + 6) % 36];
  _SN[9] = MAC_array_STA[4] % 10 + 48;
  _SN[10] = (char)((MAC_array_STA[3] * MAC_array_STA[2] + MAC_array_STA[2] * 0.86425) * 0.5) % 10 + 48;
  _SN[11] = words[((char)((MAC_array_STA[5] * 6 / MAC_array_STA[2] + MAC_array_STA[4] * 0.86425) * 0.5 + 0.5)) % 36];
  _SN[12] = '\0';
}

void GenerateSetupPassword(char *_setupID, char *_setupcode)
{
  char words[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"; // 36个字符

  _setupID[0] = words[((char)((MAC_array_STA[5] * 4.3 / MAC_array_STA[3] + MAC_array_STA[0] * 0.86425) * 0.5 + 0.5)) % 36];
  _setupID[1] = words[((char)((MAC_array_STA[1] * 2 / (MAC_array_STA[2] * 0.3) + MAC_array_STA[4] * 3) * 0.9 + 0.5)) % 36];
  _setupID[2] = words[((char)((MAC_array_STA[3] * MAC_array_STA[2] + 425) * 0.5 + 0.5)) % 36];
  _setupID[3] = words[((char)((MAC_array_STA[4] / MAC_array_STA[1] + MAC_array_STA[5] * 0.86425) * 0.5 + 25.5)) % 36];
  _setupID[4] = '\0';
  /* Unique Setup code of the format xxx-xx-xxx. Default: 111-22-333 */
  _setupcode[0] = (MAC_array_STA[0] + MAC_array_STA[5]) % 10 + 48;
  _setupcode[1] = MAC_array_STA[2] % 10 + 48;
  _setupcode[2] = ((char)((MAC_array_STA[3] * MAC_array_STA[2] + MAC_array_STA[2] * 0.86425) * 0.5) % 10) + 48;
  _setupcode[3] = 0x2d;
  _setupcode[4] = (categoryNo * MAC_array_STA[0] + MAC_array_STA[5]) % 10 + 48;
  _setupcode[5] = (categoryNo * 16 + MAC_array_STA[5] * MAC_array_STA[1]) % 10 + 48;
  _setupcode[6] = 0x2d;
  _setupcode[7] = (MAC_array_STA[0] + MAC_array_STA[1] + MAC_array_STA[2] + MAC_array_STA[3] + MAC_array_STA[4] + MAC_array_STA[5]) % 10 + 48;
  _setupcode[8] = ((MAC_array_STA[0] + MAC_array_STA[1] + MAC_array_STA[2]) * (MAC_array_STA[3] + MAC_array_STA[4] + MAC_array_STA[5])) % 10 + 48;
  _setupcode[9] = ((MAC_array_STA[0] + MAC_array_STA[5] + MAC_array_STA[3]) * (MAC_array_STA[3] + MAC_array_STA[2] + MAC_array_STA[5])) % 10 + 48;
  _setupcode[10] = '\0';
}

void outlet_in_use_setter(const homekit_value_t value)
{
  bool on = value.bool_value;
  outlet_in_use.value.bool_value = on; // sync the value

#ifdef __DEBUG
  printf("Switch: %s\n", on ? "ON" : "OFF");
#endif

  digitalWrite(PIN_Switch, on ? HIGH : LOW);
  builtinledSetStatus(PIN_BLUE_LED, outlet_in_use.value.bool_value);
}

void outlet_toggle()
{
  outlet_in_use.value.bool_value = !outlet_in_use.value.bool_value;
  outlet_in_use.setter(outlet_in_use.value);
  homekit_characteristic_notify(&outlet_in_use, outlet_in_use.value);
  digitalWrite(PIN_Switch, outlet_in_use.value.bool_value ? HIGH : LOW);
  builtinledSetStatus(PIN_BLUE_LED, outlet_in_use.value.bool_value);
}

void homekit_setup()
{
  outlet_in_use.setter = outlet_in_use_setter;
  WiFi.macAddress(MAC_array_STA);
  GenerateSerialNumber(SN);
  for (int i = EnglishWordNumber + ChineseWordNumber * 3; i < EnglishWordNumber + ChineseWordNumber * 3 + 13; i++)
  {
    if (i - (EnglishWordNumber + ChineseWordNumber * 3) < 13)
    {
      name_generation[i] = SN[i - (EnglishWordNumber + ChineseWordNumber * 3)];
    }
  }
  name.value = HOMEKIT_STRING_CPP(name_generation);
  serial_number.value = HOMEKIT_STRING_CPP(SN);
  GenerateSetupPassword(Setup_ID, Setup_code);
  Serial.println(name_generation);
  Serial.print("Setup_code:");
  Serial.print(Setup_code);
  Serial.print("  Setup_ID:");
  Serial.println(Setup_ID);
  config.password = Setup_code;
  config.setupId = Setup_ID;

  arduino_homekit_setup(&config);
}

static unsigned long next_heap_millis = 0;

void homekit_loop()
{
  btnHandler.loop();
  arduino_homekit_loop();
  static uint32_t next_heap_millis = 0;
  uint32_t time = millis();
  if (time > next_heap_millis)
  {
    SIMPLE_INFO("heap: %d, sockets: %d",
                ESP.getFreeHeap(), arduino_homekit_connected_clients_count());
    next_heap_millis = time + 5000;
  }
}
