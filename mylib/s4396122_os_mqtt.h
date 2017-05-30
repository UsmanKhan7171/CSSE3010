/**
 * @file s4396122_os_mqtt.h
 * @brief Library for publishing and subscribing to channels using mqtt
 * @author Daniel Fitzmaurice - 43961229
 * @version 1
 * @date 2017-05-31
 */
#ifndef OS_MQTT_H
#define OS_MQTT_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "MQTTClient.h"
#include "s4396122_os_print.h"

//! MQTT Task Priority
#define mqttTask_PRIORITY (tskIDLE_PRIORITY + 2)
//! MQTT Task Stack Size
#define mqttTask_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)

extern void s4396122_os_mqtt_init();
void MQTT_Task();
extern void s4396122_os_mqtt_publish(char *topic, char *message);
extern void s4396122_os_mqtt_subscribe(char *topic);
extern void s4396122_os_mqtt_set_handler(void (*handler)(char*));

#endif
