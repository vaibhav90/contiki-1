// Sets up the global package for 'Active Sensor'

#include "global-sensor.h"
#include "contiki.h"
#include "shell.h"

#include "dev/leds.h"
#include "dev/light-sensor.h"
#include "dev/sht11-sensor.h"
#include "dev/battery-sensor.h"
#include "dev/serial-line.h"
#include <stdio.h>
#include <stdlib.h>

// Global sensor_sel definition
char sensor_sel = 'l';

/*---------------------------------------------------------------------------*/
PROCESS(shell_sensor_select_process, "sensor-sel");
SHELL_COMMAND(sensor_select_command,
              "sensor-sel",
              "sensor-sel [char]: set active sensor to [char]",
              &shell_sensor_select_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(shell_sensor_select_process, ev, data) {

	static char temp_var = 'x';

	PROCESS_BEGIN();

	temp_var = shell_datatochar(data);

	if(temp_var == 'v') {
		sensor_sel = 'v';
	} else if(temp_var == 'i') {
		sensor_sel = 'i';
	} else if(temp_var == 'l') {
		sensor_sel = 'l';
	} else if(temp_var == 's') {
		sensor_sel = 's';
	} else if(temp_var == 't') {
		sensor_sel = 't';
	} else if(temp_var == 'h') {
		sensor_sel = 'h';
	} else {
		printf("Must choose one of {v,i,l,s,t,h}\n");
		printf("Currently = %c\n", sensor_sel);
	}

	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
void shell_global_sensor_init(void) {
	shell_register_command(&sensor_select_command);
}

char shell_datatochar(const char *str) {
	const char *strptr = str;

	if(str == NULL) {
		return 0;
	}

	while(*strptr == ' ') {
		++strptr;
	}

	return *strptr;
}

// Switches on the currently selected sensor.
void sensor_init(void) {
	switch(sensor_sel) {
		case 'v': 
			SENSORS_ACTIVATE(battery_sensor);
			break;
		case 'i':
			SENSORS_ACTIVATE(sht11_sensor);
			break;
		case 't':
			SENSORS_ACTIVATE(sht11_sensor);
			break;
		case 'h':
			SENSORS_ACTIVATE(sht11_sensor);
			break;
		case 'l':
			SENSORS_ACTIVATE(light_sensor);
			break;
		case 's':
			SENSORS_ACTIVATE(light_sensor);
			break;
	}
}

// Switches off the currently selected sensor.
void sensor_uinit(void) {
	switch(sensor_sel) {
		case 'v': 
			SENSORS_DEACTIVATE(battery_sensor);
			break;
		case 'i':
			SENSORS_DEACTIVATE(sht11_sensor);
			break;
		case 't':
			SENSORS_DEACTIVATE(sht11_sensor);
			break;
		case 'h':
			SENSORS_DEACTIVATE(sht11_sensor);
			break;
		case 'l':
			SENSORS_DEACTIVATE(light_sensor);
			break;
		case 's':
			SENSORS_DEACTIVATE(light_sensor);
			break;
	}
}

// Reads the currently selected sensor.
uint16_t sensor_read(void) {
	switch(sensor_sel) {
		case 'v': 
			return battery_sensor.value(0);
		case 'i':
			return sht11_sensor.value(SHT11_SENSOR_BATTERY_INDICATOR);
		case 't':
			return sht11_sensor.value(SHT11_SENSOR_TEMP);
		case 'h':
			return sht11_sensor.value(SHT11_SENSOR_HUMIDITY);
		case 'l':
			return light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC);
		case 's':
			return light_sensor.value(LIGHT_SENSOR_TOTAL_SOLAR);
	}

	// Else error, return 0
	return 0;
}

