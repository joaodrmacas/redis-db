#ifndef __STRUCTURES_HPP__
#define __STRUCTURES_HPP__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

//Cached hash value for resizing and fast lookup
typedef struct Node {
    Node *next = NULL;
    uint64_t hash_value = 0;
}Node;

//Resizable hash table
typedef struct HashTable {
    Node **table = NULL;
    size_t cur_size = 0;
    size_t mask = 0; //2^size - 1
} HashTable;

#endif