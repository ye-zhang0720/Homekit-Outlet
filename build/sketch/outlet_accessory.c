#line 1 "/Users/zhangye/Documents/智能家居/HomekitOutlet/outlet_accessory.c"
/*
 * simple_led_accessory.c
 * Define the accessory in pure C language using the Macro in characteristics.h
 *
 *  Created on: 2020-02-08
 *      Author: Mixiaoxiao (Wang Bin)
 *  Edited on: 2020-03-01
 *      Edited by: euler271 (Jonas Linn)
 */

#include <Arduino.h>
#include <homekit/types.h>
#include <homekit/homekit.h>
#include <homekit/characteristics.h>
#include <stdio.h>
#include <port.h>

//const char * buildTime = __DATE__ " " __TIME__ " GMT";

#define ACCESSORY_NAME  ("ESP8266_Outlet")
#define ACCESSORY_SN  ("SN_0123456")  //SERIAL_NUMBER
#define ACCESSORY_MANUFACTURER ("西北偏北工作室")
#define ACCESSORY_MODEL  ("ESP8266")





homekit_characteristic_t name = HOMEKIT_CHARACTERISTIC_(NAME, ACCESSORY_NAME);
homekit_characteristic_t serial_number = HOMEKIT_CHARACTERISTIC_(SERIAL_NUMBER, ACCESSORY_SN);
homekit_characteristic_t outlet_in_use = HOMEKIT_CHARACTERISTIC_(ON,false);

/*
void outlet_update() {
	if (outlet_status) {
        printf("ON\n");
        digitalWrite(PIN_Outlet, LOW);
	} else {
		printf("OFF\n");
		digitalWrite(PIN_Outlet, HIGH);
	}
}
*/





void accessory_identify(homekit_value_t _value) {
	printf("accessory identify\n");
	/*for (int j = 0; j < 3; j++) {
		led_power = true;
		led_update();
		delay(100);
		outlet_status = false;
		led_update();
		delay(100);
	}*/
}

homekit_accessory_t *accessories[] =
		{
				HOMEKIT_ACCESSORY(
						.id = 1,
						.category = homekit_accessory_category_outlet,
						.services=(homekit_service_t*[]){
						  HOMEKIT_SERVICE(ACCESSORY_INFORMATION,
						  .characteristics=(homekit_characteristic_t*[]){
						    &name,
						    HOMEKIT_CHARACTERISTIC(MANUFACTURER, ACCESSORY_MANUFACTURER),
						    &serial_number,
						    HOMEKIT_CHARACTERISTIC(MODEL, ACCESSORY_MODEL),
						    HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "0.0.1"),
						    HOMEKIT_CHARACTERISTIC(IDENTIFY, accessory_identify),
						    NULL
						  }),


                        HOMEKIT_SERVICE(OUTLET, .primary=true,
						  .characteristics=(homekit_characteristic_t*[]){
						    HOMEKIT_CHARACTERISTIC(NAME, "Outlet"),
						    &outlet_in_use,
                            &name,
						    NULL
						  }),
						  NULL
						}),

				NULL
		};

homekit_server_config_t config = {
		.accessories = accessories,
		.password = "111-11-111",
		//.on_event = on_homekit_event,
		.setupId = "ABCD"
};

