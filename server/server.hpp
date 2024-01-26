#ifndef __SERVER_HPP__
#define __SERVER_HPP__

#include "../utility.hpp"
#include "conn.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <vector>
#include <poll.h>

#define TIMEOUT 1000

typedef struct Conn Conn;

int start_server();
bool try_handle_request(Conn* conn);
int32_t do_request(const uint8_t *req, uint32_t reqlen, uint8_t *res ,uint32_t *rescode, uint32_t reslen);
int32_t parse_req(const uint8_t *data,size_t len, std::vector<std::string> &out);

#endif