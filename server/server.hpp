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
int handle_request(int fd);
bool try_handle_request(Conn* conn);

#endif