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

enum {
    RES_OK = 0,
    RES_ERR = 1,
    RES_NOK = 2,
};

typedef struct Conn Conn;

int start_server();
bool try_handle_request(Conn* conn);
int32_t do_request(const uint8_t *req, uint32_t reqlen, uint8_t *res ,uint32_t *rescode, uint32_t *reslen);
int32_t parse_req(const uint8_t *data,size_t len, std::vector<std::string> &out);


uint32_t get_cmd(const std::vector<std::string> &cmd, uint8_t *res, uint32_t *reslen);
uint32_t set_cmd(const std::vector<std::string> &cmd, uint8_t *res, uint32_t *reslen);
uint32_t del_cmd(const std::vector<std::string> &cmd, uint8_t *res, uint32_t *reslen);

#endif