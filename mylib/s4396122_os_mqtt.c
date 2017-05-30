#include "s4396122_os_mqtt.h"

void s4396122_os_mqtt_init() {
    xTaskCreate(&MQTT_Task, "MQTT", configMINIMAL_STACK_SIZE * 8, NULL, mqttTask_PRIORITY, NULL);
}

void MQTT_Task(void) {

    char buff[100];
    char readBuff[100];
    
    Network network;
    NewNetwork(&network);
    ConnectNetwork(&network, "192.168.0.1", 1883);
    debug_printf("Socket Connected\n");

    Client client;
    MQTTClient(&client, &network, 1000, buff, 100, readBuff, 100);
    debug_printf("Created Client\n");
    TaskHandle_t taskHandle = xTaskGetHandle("MQTTRD");
    if (taskHandle == NULL) {
        debug_printf("Error\n");
    }
    vTaskSuspend(taskHandle);
    debug_printf("Waiting\n");
    vTaskDelay(5000);

    MQTTConnect(&client, NULL);
    debug_printf("Connected\n");

    while(1);
}


void s4396122_os_mqtt_publish(char *topic, char *message) {
}

/**
 * @brief Message handler for incoming (published) messages.
 *
 * @param data MessageData struct pointer. Contains Qos info and payload
 */
void mqtt_messagehandler(MessageData *data) {
}
