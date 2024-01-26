#include "client.hpp"


int connect_to_server(){
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) die("socket");

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234); //Port selected: 1234
    addr.sin_addr.s_addr = ntohl(0); //Ip address selected: 127.0.0.1
    int rv = connect(fd, (struct sockaddr *)&addr, sizeof(addr));

    if (rv){
        die("connect");
    }

    return fd;
}

static int32_t write_req(int fd, const std::vector<std::string> &cmd){

    uint32_t len = 4;

    for (const std::string &s : cmd){
        len += 4 + s.size();
    }

    if (len > MSG_LEN){
        printf("Message too long\n");
        return -1;
    }

    //Write message lenght
    char wbuf[HEADER_LEN + MSG_LEN];
    memcpy(wbuf, &len, HEADER_LEN);
    uint32_t n = cmd.size();
    memcpy(wbuf + HEADER_LEN, &n, HEADER_LEN);

    size_t cur = 8;
    for (const std::string &s : cmd){
        uint32_t p = (uint32_t) s.size();
        memcpy(wbuf + cur, &p, HEADER_LEN);
        memcpy(wbuf + cur + HEADER_LEN, s.data(), s.size());
        cur += HEADER_LEN + s.size();
    }
    
    return write_full(fd, wbuf, len + HEADER_LEN);
}

static int32_t read_res(int fd){
    //Read response
    char rbuf[HEADER_LEN + MSG_LEN + 1];
    errno = 0;
    int32_t err = read_full(fd, rbuf, HEADER_LEN);
    if (err){
        if (errno == 0){
            printf("EOF\n");
        }
        else {
            printf("read error\n");
        }
        return err;
    }

    uint32_t len=0;
    //Copy the first bytes for len
    memcpy(&len,rbuf,HEADER_LEN);
    if (len > MSG_LEN){
        printf("message too long\n");
        return -1;
    }

    err = read_full(fd, rbuf+HEADER_LEN, len);
    if (err){
        printf("read error\n");
        return err;
    }

    uint32_t rescode = 0;
    if (len < 4){
        printf("message too short\n");
        return -1;
    }

    memcpy(&rescode, rbuf+HEADER_LEN, HEADER_LEN);

    rbuf[HEADER_LEN + len] = '\0';
    printf("Message: [%u] %s\n", rescode, rbuf+HEADER_LEN+HEADER_LEN);
    return 0;
}


int main(int argc, char **argv){

    int fd = connect_to_server();

    std::vector<std::string> cmd;
    for (int i = 1; i < argc; ++i) {
        cmd.push_back(argv[i]);
    }
    int32_t err = write_req(fd, cmd);
    if (err) {
        goto L_DONE;
    }
    err = read_res(fd);
    if (err) {
        goto L_DONE;
    }
    

L_DONE:
    close(fd);
    return 0;
}
