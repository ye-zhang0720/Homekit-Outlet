/*
 * simple_led_accessory.c
 * Define the accessory in pure C language using the Macro in characteristics.h
 *
 *  Created on: 2021-09-19
 *      Author: ye zhang
 * 
 */

#include <Arduino.h>
#include <homekit/types.h>
#include <homekit/homekit.h>
#include <homekit/characteristics.h>
#include <stdio.h>
#include <port.h>


#define ACCESSORY_NAME  ("智能插座SN_79B8497R")
#define ACCESSORY_SN  ("SN_0123456")
#define ACCESSORY_MANUFACTURER ("西北偏北工作室")
#define ACCESSORY_MODEL  ("XBPB002")


homekit_characteristic_t name = HOMEKIT_CHARACTERISTIC_(NAME, ACCESSORY_NAME);
homekit_characteristic_t outlet_name = HOMEKIT_CHARACTERISTIC_(NAME, "智能插座");
homekit_characteristic_t serial_number = HOMEKIT_CHARACTERISTIC_(SERIAL_NUMBER, ACCESSORY_SN);
homekit_characteristic_t outlet_in_use = HOMEKIT_CHARACTERISTIC_(ON, false);

void accessory_identify(homekit_value_t _value)
{
    printf("accessory identify\n");
}

homekit_accessory_t *accessories[] =
    {
        HOMEKIT_ACCESSORY(
                .id = 1,
                .category = homekit_accessory_category_outlet,
                .services = (homekit_service_t *[]){
                    HOMEKIT_SERVICE(ACCESSORY_INFORMATION,
                                    .characteristics = (homekit_characteristic_t *[]){
                                    &name,
						            HOMEKIT_CHARACTERISTIC(MANUFACTURER, ACCESSORY_MANUFACTURER),
						            &serial_number,
						            HOMEKIT_CHARACTERISTIC(MODEL, ACCESSORY_MODEL),
						            HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "0.1.0"),
						            HOMEKIT_CHARACTERISTIC(IDENTIFY, accessory_identify),
                                    NULL}),

                    HOMEKIT_SERVICE(OUTLET, .primary = true, .characteristics = (homekit_characteristic_t *[]){
                        &outlet_in_use, 
                        &outlet_name, 
                        NULL}),
                    NULL}),
        NULL};


homekit_server_config_t config = {
    .accessories = accessories,
    .password = "584-42-597",
    .setupId = "R679",
    //.on_event = on_homekit_event,
    };
