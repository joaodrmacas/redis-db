
#include "server.hpp"
#include "structures/structures.hpp"
#include <map>

HashMap db;


//Fowler–Noll–Vo hash function
static uint64_t str_hash(const uint8_t *str, size_t len){
    uint32_t h = 0x811C9DC5;
    for (size_t i=0; i<len;i++){
        h = (h+str[i])*0x01000193;
    }
    return h;
}

static bool entry_eq(Node* n1, Node* n2){
    Entry *left = container_of(n1,Entry,node);
    Entry *right = container_of(n2,Entry,node);
    return left->key == right->key;
}

uint32_t get_cmd(const std::vector<std::string> &cmd, uint8_t *res, uint32_t *reslen){

    printf("Performing get\n");

    Entry key;
    key.key = cmd[1];
    key.node.hash_value = str_hash((uint8_t *)key.key.data(), key.key.size());

    Node *node = hashmap_lookup(&db, &key.node,entry_eq);

    if (!node){
        printf("get_cmd: key not found\n");
        return RES_NOK;
    }

    std::string &val = container_of(node,Entry,node)->value;

    if (val.size() > MSG_LEN){
        printf("get_cmd: value too long\n");
        return RES_ERR;
    }

    memcpy(res,val.c_str(),val.size());
    *reslen = (uint32_t) val.size();
    return RES_OK;
} 

uint32_t set_cmd(const std::vector<std::string> &cmd, uint8_t *res, uint32_t *reslen){

    printf("Performing set\n");
    
    (void) res;
    (void) reslen;
    
    Entry key;
    key.key = cmd[1];
    key.node.hash_value = str_hash((uint8_t *)key.key.data(), key.key.size());

    Node *node = hashmap_lookup(&db, &key.node,entry_eq);


    if (node){
        printf("set_cmd: key already exists\n");
        memcpy(res,"Key already exists",18);
        *reslen = 18;
        return RES_NOK;
    }


    Entry *entry = new Entry();
    entry->key = key.key;
    entry->value = cmd[2];
    entry->node.hash_value = key.node.hash_value;

    hashmap_insert(&db, &entry->node);

    return RES_OK;
} 

uint32_t del_cmd(const std::vector<std::string> &cmd, uint8_t *res, uint32_t *reslen){

    printf("Performing del\n");
    
    (void) res;
    (void) reslen;

    Entry key;
    key.key = cmd[1];
    key.node.hash_value = str_hash((uint8_t *)key.key.data(), key.key.size());

    Node *node = hashmap_delete(&db, &key.node,entry_eq);
    if (!node){
        printf("del_cmd: key not found\n");
        memcpy(res,"Key not found",13);
        *reslen = 13;
        return RES_NOK;
    }

    delete container_of(node,Entry,node);
    return RES_OK;
} 