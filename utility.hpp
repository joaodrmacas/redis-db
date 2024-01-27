#ifndef __UTILITY_HPP__
#define __UTILITY_HPP__

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <iostream>

#define MSG_LEN 4096
#define HEADER_LEN 4

enum {
    SER_NIL = 0, //NULL
    SER_ERR = 1, //Error
    SER_STR = 2, //OK
    SER_INT = 3, //Integer 64bits
    SER_ARR = 4, //Array
};

int32_t read_full(int fd,char* buf, size_t n);
int32_t write_full(int fd, const char* buf, size_t n);
void die(const char *s);
void set_fd_nb(int fd);

#endif