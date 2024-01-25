#ifndef __CONN_HPP__
#define __CONN_HPP__

#include "../utility.hpp"
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <poll.h>
#include "server.hpp"

enum {
    STATE_REQ = 0,
    STATE_RES = 1,
    STATE_END = 2,
};

typedef struct Conn {
    int fd = -1;
    uint32_t state = STATE_REQ;
    size_t rbuf_size = 0;
    uint8_t rbuf[HEADER_LEN + MSG_LEN];

    size_t wbuf_size = 0;
    size_t wbuf_sent = 0;
    uint8_t wbuf[HEADER_LEN + MSG_LEN];
} Conn;

void connection_io(Conn *conn);
void conn_put(std::vector<Conn *> &fd2conn, Conn *conn);
int32_t accept_new_conn(std::vector<Conn *> &fd2conn, int fd);
void state_res(Conn* conn);
void state_req(Conn* conn);

#endif