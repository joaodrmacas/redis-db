#include "server.hpp"

void out_nil(std::string &out){
    out.push_back(SER_NIL);
}

void out_err( std::string &out, int32_t code ,const std::string &msg){
    out.push_back(SER_ERR);
    out.append((char*) &code,HEADER_LEN);
    uint32_t len = (uint32_t) msg.size();
    out.append((char*) &len,HEADER_LEN);
    out.append(msg);
}

void out_int( std::string &out, int64_t val){
    out.push_back(SER_INT);
    out.append((char*) &val,8);
}

void out_str( std::string &out, const std::string &str){
    out.push_back(SER_STR);
    uint32_t len = (uint32_t) str.size();
    out.append((char*) &len,HEADER_LEN);
    out.append(str);
}

void out_arr (std::string &out, uint32_t n){
    out.push_back(SER_ARR);
    out.append((char*) &n,4);
}