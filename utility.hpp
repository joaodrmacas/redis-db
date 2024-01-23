#ifndef __UTILITY_HPP__
#define __UTILITY_HPP__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <iostream>

#define MSG_LEN 4096
#define HEADER_LEN 4

int32_t read_full(int fd,char* buf, size_t n);
int32_t write_full(int fd, const char* buf, size_t n);
void die(const char *s);

#endif