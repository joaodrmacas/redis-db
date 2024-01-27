
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

void get_cmd(const std::vector<std::string> &cmd, std::string &out){

    printf("Performing get\n");

    Entry key;
    key.key = cmd[1];
    key.node.hash_value = str_hash((uint8_t *)key.key.data(), key.key.size());

    Node *node = hashmap_lookup(&db, &key.node,entry_eq);

    if (!node){
        printf("get_cmd: key not found\n");
        return out_err(out,ERR_KEY_NOT_EXIST,"Key not found");
    }

    std::string &val = container_of(node,Entry,node)->value;

    if (val.size() > MSG_LEN){
        printf("get_cmd: value too long\n");
        return out_err(out,ERR_2BIG,"Value too long");
    }

    out_str(out,val);
} 

void set_cmd(const std::vector<std::string> &cmd,  std::string &out){

    printf("Performing set\n");
    
    Entry key;
    key.key = cmd[1];
    key.node.hash_value = str_hash((uint8_t *)key.key.data(), key.key.size());

    Node *node = hashmap_lookup(&db, &key.node,entry_eq);


    if (node){
        printf("set_cmd: key already exists\n");
        return out_err(out,ERR_KEY_EXIST,"Key already exists");
    }


    Entry *entry = new Entry();
    entry->key = key.key;
    entry->value = cmd[2];
    entry->node.hash_value = key.node.hash_value;

    hashmap_insert(&db, &entry->node);

    return out_nil(out);
} 

void del_cmd(const std::vector<std::string> &cmd,  std::string &out){

    printf("Performing del\n");
    
    Entry key;
    key.key = cmd[1];
    key.node.hash_value = str_hash((uint8_t *)key.key.data(), key.key.size());

    Node *node = hashmap_delete(&db, &key.node,entry_eq);
    if (!node){
        return out_err(out,ERR_KEY_NOT_EXIST,"Key not found");
    }

    delete container_of(node,Entry,node);
    return out_int(out, node ? 1 : 0);
}

void keys_scan(Node* node, void *arg){
    std::string &out = *(std::string*)arg;
    Entry *entry = container_of(node,Entry,node);
    out_str(out,entry->key);
}

void keys_cmd(const std::vector<std::string> &cmd,  std::string &out){

    printf("Performing keys\n");

    out_arr(out,hashmap_total_size(&db));
    hashmap_scan(&db,&keys_scan,&out);
}
