#include "s4396122_hal_tcp.h"

int sock;

void s4396122_hal_tcp_init() {
    s4396122_util_print_debug("Starting Network");

    sock = 0;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        s4396122_util_print_error("Can not create socket");
        return;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(TCP_SOCKET_PORT);
    address.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (const struct sockaddr *) &address, sizeof(address)) < 0) {
        s4396122_util_print_error("Can not bind socket");
        return;
    }

    listen(sock, 5);
}

struct tcpConnection s4396122_hal_tcp_accept() {
    struct sockaddr_in remotehost;
    long size = sizeof(struct sockaddr_in);
    int tcpconn = accept(sock, (struct sockaddr *) &remotehost, (socklen_t *) &size);
    struct tcpConnection conn;
    if (tcpconn == -1) {
        s4396122_util_print_error("Error connecting");
    }
    s4396122_util_print_info("Got a new connection");
    conn.fp = tcpconn;
    conn.input = s4396122_util_int_queue_create();
    conn.open = 1;
    return conn;
}

void s4396122_hal_tcp_read(struct tcpConnection *conn, void (*f)(IntQueue *)) {
    if (conn->open) {
        int ret;
        unsigned char buffer[20];
        memset(buffer, 0, sizeof(buffer));
        if ((ret = read(conn->fp, buffer, sizeof(buffer))) > 0) {
            for (int i = 0; i < ret; i++) {
                if (buffer[i] == '\n') {
                    f(conn->input);
                    s4396122_util_int_queue_free(conn->input);
                    conn->input = s4396122_util_int_queue_create();
                    continue;
                }
                s4396122_util_int_queue_push(conn->input, buffer[i]);
            }
        } else {
            s4396122_util_print_error("Connection closed");
            s4396122_util_int_queue_free(conn->input);
            conn->open = 0;
            return;

        }
    }
}

void s4396122_hal_tcp_print(struct tcpConnection *conn, IntQueue *message) {
    int size = s4396122_util_int_queue_size(message);
    unsigned char buffer[size+1];
    for (int i = 0; i < size; i++) {
        buffer[i] = s4396122_util_int_queue_pop(message);
    }
    buffer[size] = '\0';
    write(conn->fp, buffer, size);
}
