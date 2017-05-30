/**
 * @file s4396122_os_mqtt.c
 * @brief Library for publishing and subscribing to channels using mqtt
 * @author Daniel Fitzmaurice - 43961229
 * @version 1
 * @date 2017-05-31
 */
#include "s4396122_os_mqtt.h"

Client client;              //!< Client connection to mqtt server
int connected = 0;          //!< If connected to mqtt server
void (*msgHandler)(char *); //!< External handler for incoming messages

void mqtt_messagehandler(MessageData *data);

/**
 * @brief Initializes the MQTT library peripherals
 */
void s4396122_os_mqtt_init() {

    xTaskCreate(&MQTT_Task, "MQTT", configMINIMAL_STACK_SIZE * 8, NULL, 
            mqttTask_PRIORITY, NULL);
}

/**
 * @brief Handles connecting to a network, this was adapted from the provided
 * source
 *
 * @param n     Network structure to connect with
 * @param addr  Address to connect to
 * @param port  Port to connect on
 */
void connect_network(Network *n, char *addr, int port) {

    int addrSize;
    struct sockaddr_in sAddr;

    /* Create a TCP Socket */
    if ((n->my_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {

        debug_printf("can not create socket");
        return;
    }

    sAddr.sin_family = AF_INET;
    sAddr.sin_port = htons((unsigned short) port);
    sAddr.sin_addr.s_addr = inet_addr(addr);

    if (connect(n->my_socket, (struct sockaddr *)(&sAddr), sizeof (sAddr)) 
            < 0) {

        debug_printf("MQTT ERROR connecting");
        return;
    }
}

/**
 * @brief Handles reading a packet of data from the input. This is overridden 
 * to prevent errors occurring
 *
 * @param n         Network connection to read from
 * @param buffer    Buffer to add input to
 * @param len       Amount of data to read
 * @param timeout   Not currently used
 *
 * @return Not currently used
 */
int network_read(Network *n, char *buffer, int len, int timeout) {

    read(n->my_socket, buffer, len);
}

/**
 * @brief Task for handling MQTT functionality
 */
void MQTT_Task(void) {

    char buff[100];
    char readBuff[100];

    // Create a network connection and connect to the server
    Network network;
    NewNetwork(&network);
    connect_network(&network, "192.168.0.1", 1883);

    // Initialize the buffers and set the handlers
    MQTTClient(&client, &network, 1000, buff, 100, readBuff, 100);
    client.ipstack->mqttread = network_read;
    client.defaultMessageHandler = mqtt_messagehandler;

    // Send the CONNECT MQTT request
    MQTTConnect(&client, NULL);
    connected = 1;

    // Publish status flags and connect to the cmd channel
    s4396122_os_mqtt_publish("debug", "MQTT Init");
    s4396122_os_mqtt_subscribe("cmd");
    s4396122_os_mqtt_publish("debug", "MQTT Subbed");

    while(1) {

        Timer timer;
        InitTimer(&timer);
        countdown_ms(&timer, 1000);
        cycle(&client, &timer);

        s4396122_os_mqtt_publish("debug", "Got Command");
        vTaskDelay(100);
    }
}

/**
 * @brief Publishes a message to the MQTT server
 *
 * @param inTopic   Topic to public the message under
 * @param message   Message to send
 */
void s4396122_os_mqtt_publish(char *inTopic, char *message) {

    if (!connected) {

        return;
    }

    // Append the topic to np2 so that np2 is the root of all channels coming 
    // from the np2
    char topic[50];
    memset(topic, 0, sizeof(topic));
    sprintf(topic, "np2/%s", inTopic);

    // Create a new message and publish it
    MQTTMessage new_message;
    new_message.payloadlen = strlen(message);
    new_message.payload = message;
    new_message.qos = QOS0;
    MQTTPublish(&client, topic, &new_message);
}

/**
 * @brief Message handler for incoming (published) messages.
 *
 * @param data MessageData struct pointer. Contains Qos info and payload
 */
void mqtt_messagehandler(MessageData *data) {

    char buffer[data->message->payloadlen + 2];
    memset(buffer, 0, sizeof(buffer));
    strncpy(buffer, data->message->payload, data->message->payloadlen);
    if (msgHandler != NULL) {

        msgHandler(buffer);
    }
}

/**
 * @brief Assigns the external message handler
 *
 * @param handler   New external handler to be assigned
 */
void s4396122_os_mqtt_set_handler(void (*handler)(char*)) {

    msgHandler = handler;
}

/**
 * @brief Subscribes to a channel
 *
 * @param inTopic   Channel to subscribe to
 */
void s4396122_os_mqtt_subscribe(char *inTopic) {

    if (!connected) {

        return;
    }

    char topic[50];
    memset(topic, 0, sizeof(topic));
    sprintf(topic, "np2/%s", inTopic);
    if (MQTTSubscribe(&client, topic, QOS0, mqtt_messagehandler) == FAILURE) {

        debug_printf("Error\n");
    }
}
