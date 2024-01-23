#include "server.hpp"


int start_server(){

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) die("socket");

    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234); //Port selected: 1234
    addr.sin_addr.s_addr = ntohl(0); //Ip address selected: 0.0.0.0

    // IP: 0.0.0.0:1234

    int err = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
    if (err < 0) die("bind");

    err = listen(fd,SOMAXCONN);
    if (err<0) die("listen");

    return fd;
}

int handle_request(int fd){

    printf("Handling request...\n");

    char rbuf[HEADER_LEN + MSG_LEN + 1];
    errno = 0;
    uint32_t len = 0;


    //Read message header
    int32_t err = read_full(fd, rbuf, HEADER_LEN);

    if (err<0) {
        if (errno == 0){
            printf("EOF\n");
        }
        else {
            printf("read error\n");
        }
        return err;
    }

    //Copy the first bytes for len
    memcpy(&len,rbuf,HEADER_LEN);
    printf("Length: %d | ", len);

    if (len > MSG_LEN){
        printf("message too long\n");
        return -1;
    }

    err = read_full(fd, rbuf, len);
    if (err){
        printf("read error\n");
        return err;
    }

    rbuf[HEADER_LEN + len] = '\0';
    printf("Message: %s |\n\n", rbuf);

    //Reply

    const char reply[] = "World";
    char wbuf[HEADER_LEN + sizeof(reply) + 1];
    len = (uint32_t) sizeof(reply);

    memcpy(wbuf, &len, HEADER_LEN);
    memcpy(&wbuf[HEADER_LEN], reply, len);
    return write_full(fd, wbuf, HEADER_LEN + len);
}

int main(){
    
    int sv_fd=0, conn_fd=0;

    sv_fd = start_server();

    while (true){

        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        conn_fd = accept(sv_fd, (struct sockaddr *)&client_addr, &client_len);

        if (conn_fd < 0) continue; //If a user fails to join, continue to the next user


        while (true){
            int32_t err = handle_request(conn_fd);

            if (err<0) break; //If a user fails to join, continue to the next user

        }

        printf("Closing server...\n");
        close(conn_fd);
        break;
    }

}