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

static int32_t write_req(int fd, const char *text){

    uint32_t len = (uint32_t) strlen(text);

    if (len > MSG_LEN){
        printf("Message too long\n");
        return -1;
    }

    //Write message lenght
    char wbuf[HEADER_LEN + MSG_LEN];
    memcpy(wbuf, &len, HEADER_LEN);

    //Write message
    memcpy(wbuf + HEADER_LEN, text, len);
    if (int32_t err = write_full(fd,wbuf,4 + len)){
        return err;
    }
    return 0;
}

static int32_t read_res(int fd){
    //Read response
    uint32_t len=0;
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

    rbuf[HEADER_LEN + len] = '\0';
    printf("Message: %s\n", rbuf+HEADER_LEN);
    return 0;
}


int main(){

    printf("Started\n");
    int fd = connect_to_server();

    const char *query_list[3] = {"Hello1", "Hello2", "Hello3"};
    printf("Started2\n");


    for (size_t i = 0; i < 3; i++){
        int32_t err = write_req(fd, query_list[i]);
        if (err){
            goto L_DONE;
        }
    }

    printf("Started3\n");


    for (size_t i=0; i<3; i++){
        int32_t err = read_res(fd);
        if (err){
            goto L_DONE;
        }
    }

    printf("Started4\n");
    
    close(fd);
    
    

L_DONE:
    close(fd);
    return 0;
}
