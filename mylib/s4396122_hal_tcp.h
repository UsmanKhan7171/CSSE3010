#ifndef HAL_TCP_H
#define HAL_TCP_H

#include "FreeRTOS.h"
#include "netconfig.h"
#include "task.h"
#include "s4396122_util_print.h"
#include "s4396122_util_int_queue.h"

#include "tcpip.h"
#include "lwip/opt.h"
#include "lwip/api.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define tcpConnection_PRIORITY (tskIDLE_PRIORITY + 2)
#define tcpConnection_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)
#define TCP_SOCKET_PORT 10

struct tcpConnection {
    int fp;
    IntQueue *input;
    int open;
};

extern void s4396122_hal_tcp_init();
extern struct tcpConnection s4396122_hal_tcp_accept();
extern void s4396122_hal_tcp_read(struct tcpConnection *conn, void (*f)(IntQueue *));
extern void s4396122_hal_tcp_print(struct tcpConnection *conn, IntQueue *message);

#endif
