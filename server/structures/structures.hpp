#ifndef __STRUCTURES_HPP__
#define __STRUCTURES_HPP__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <cstddef>

#define HT_INIT_SIZE 4
#define MAX_LOAD_FACTOR 8
#define RESIZING_WORK 128


//Returns a pointer to the Entry structure that contains the node
#define container_of(ptr, type, member) \
    reinterpret_cast<type*>(reinterpret_cast<char*>(ptr) - offsetof(type, member))



//Cached hash value for resizing and fast lookup
typedef struct Node {
    Node *next = NULL;
    uint64_t hash_value = 0;
}Node;

//Structure for key
typedef struct Entry {
    Node node;
    std::string key;
    std::string value;
} Entry;

//Resizable hash table
typedef struct HashTable {
    Node **table = NULL;
    size_t cur_size = 0;
    size_t mask = 0; //2^size - 1
} HashTable;

//HashMap Interface with two hash tables for dynamic resizing
typedef struct HashMap {
    HashTable t1;
    HashTable t2;
    size_t resizing_index = 0; 
} HashMap;




void hashmap_insert(HashMap *hm, Node *node);
Node* hashmap_lookup(HashMap *hm, Node *key, bool (*eq) (Node*, Node*));
Node* hashmap_delete(HashMap *hm, Node *key, bool (*eq) (Node*, Node*));
void hashmap_scan(HashMap *hm, void (*func) (Node* , void* ), void *arg);
uint32_t hashmap_total_size(HashMap *hm);
void hashmap_destroy(HashMap *hm);


#endif