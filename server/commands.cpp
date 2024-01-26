
#include "server.hpp"
#include <map>

static std::map<std::string,std::string> db;

uint32_t get_cmd(const std::vector<std::string> &cmd, uint8_t *res, uint32_t *reslen){

    if ( !db.count(cmd[1])){
        printf("get_cmd: key not found\n");
        return RES_NOK;
    }

    std::string &val = db[cmd[1]];

    if (val.size() > MSG_LEN){
        printf("get_cmd: value too long\n");
        return RES_ERR;
    }

    memcpy(res, val.c_str(), val.size());
    *reslen = (uint32_t) val.size();
    return RES_OK;
} 

uint32_t set_cmd(const std::vector<std::string> &cmd, uint8_t *res, uint32_t *reslen){
    
    if ( db.count(cmd[1])){
        printf("set_cmd: key already exists\n");
        return RES_NOK;
    }

    (void) res;
    (void) reslen;

    db[cmd[1]] = cmd[2];
    return RES_OK;
    
} 

uint32_t del_cmd(const std::vector<std::string> &cmd, uint8_t *res, uint32_t *reslen){
    
    if ( !db.count(cmd[1])){
        printf("del_cmd: key not found\n");
        return RES_NOK;
    }

    (void) res;
    (void) reslen;

    db.erase(cmd[1]);
    return RES_OK;
} 