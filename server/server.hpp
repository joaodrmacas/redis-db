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


//Response codes
enum {
    RES_OK = 0,
    RES_ERR = 1,
    RES_NOK = 2,
};

//Error codes
enum {
    ERR_UNK = 0,
    ERR_2BIG = 1,
    ERR_ARGS = 2,
    ERR_KEY_EXIST = 3,
    ERR_KEY_NOT_EXIST = 4,
};

typedef struct Conn Conn;

int start_server();
bool try_handle_request(Conn* conn);
int32_t do_request(const std::vector<std::string> &cmd, std::string &out);
int32_t parse_req(const uint8_t *data,size_t len, std::vector<std::string> &cmd);

void get_cmd(const std::vector<std::string> &cmd, std::string &out);
void set_cmd(const std::vector<std::string> &cmd, std::string &out);
void del_cmd(const std::vector<std::string> &cmd, std::string &out);
void keys_cmd(const std::vector<std::string> &cmd,  std::string &out);

void out_nil(std::string &out);
void out_err( std::string &out, int32_t code ,const std::string &msg);
void out_int( std::string &out, int64_t val);
void out_str( std::string &out, const std::string &str);
void out_arr (std::string &out, uint32_t n);

#endif