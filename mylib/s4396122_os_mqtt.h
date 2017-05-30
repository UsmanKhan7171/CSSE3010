#ifndef OS_MQTT_H
#define OS_MQTT_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "MQTTClient.h"
#include "s4396122_os_print.h"

#define print debug_printf

#define mqttTask_PRIORITY (tskIDLE_PRIORITY + 2)
#define mqttTask_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)

#define OS_MQTT_BROKER_IP "192.168.0.1"
#define OS_MQTT_BROKER_PORT 1883

extern void s4396122_os_mqtt_init();
void MQTT_Task();
extern void s4396122_os_mqtt_publish(char *topic, char *message);
extern void s4396122_os_mqtt_subscribe(char *topic);
extern void s4396122_os_mqtt_set_handler(void (*handler)(char*));

#endif
