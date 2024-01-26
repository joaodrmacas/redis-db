#include "server.hpp"

/* TODO

 - Try to use epoll instead of poll in the event loop. This should be easy.

 - We are using memmove to reclaim read buffer space. 
However, memmove on every request is unnecessary, 
change the code the perform memmove only before read.

 - In the state_res function, write was performed for a single response.
 In pipelined sceneries, we could buffer multiple responses
 and flush them in the end with a single write call. 
 Note that the write buffer could be full in the middle.

*/


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

bool try_handle_request(Conn* conn){

    //Not enough data on the buffer
    if (conn->rbuf_size < HEADER_LEN)
        return false;

    uint32_t len;
    memcpy(&len, conn->rbuf, HEADER_LEN);

    if (len > MSG_LEN){
        printf("try_handle_request: message too long\n");
        conn->state = STATE_END;
        return false;
    }

    //Not enough data on the buffer
    if (conn->rbuf_size < HEADER_LEN + len)
        return false;

    printf("Message received: %s\n", &conn->rbuf[HEADER_LEN]);

    //Reply

    uint32_t rescode = 0;
    uint32_t reslen = 0;
    int32_t err = do_request(&conn->rbuf[HEADER_LEN],len,&conn->wbuf[HEADER_LEN+HEADER_LEN],&rescode,&reslen);

    if (err){
        printf("try_handle_request: do_request failed\n");
        conn->state = STATE_END;
        return -1;
    }

    reslen += HEADER_LEN;
    memcpy(&conn->wbuf[0], &reslen, HEADER_LEN);
    memcpy(&conn->wbuf[HEADER_LEN], &rescode, HEADER_LEN);
    conn->wbuf_size = reslen + HEADER_LEN;

    size_t remain = conn->rbuf_size - (HEADER_LEN + len);
    if (remain){
        memmove(conn->rbuf, &conn->rbuf[HEADER_LEN + len], remain);
    }
    conn->rbuf_size = remain;

    conn->state = STATE_RES;
    state_res(conn);

    //If the the request was succescfully handled, continue the loop.
    //(When STATE == STATE_REQ then it was successful)
    return (conn->state == STATE_REQ);
}

int32_t do_request(const uint8_t *req, uint32_t reqlen, uint8_t *res ,uint32_t *rescode, uint32_t reslen){

    //vector with each argument of the request
    std::vector<std::string> cmd;

    if (parse_req(req,reqlen,cmd) != 0){
        printf("do_request: request with bad format\n");
        return -1;
    }

    if (cmd.size() == 2 && (cmd[0] == "GET" || cmd[0] == "get")){
        *rescode = get_cmd();
    }
    else if (cmd.size() == 3 && (cmd[0] == "SET" || cmd[0] == "set")){
        *rescode = set_cmd();
    }
    else if (cmd.size() == 1 && (cmd[0] == "DEL" || cmd[0] == "del")){
        *rescode = del_cmd();
    }
    else{
        //Unknown command
        *rescode = RES_ERR;
        const char *msg = "Unknown command\n";
        strcpy((char *)res,msg);
        reslen = strlen(msg);
        return 0;
    }

    return 0;
}

int32_t parse_req(const uint8_t *data,size_t len, std::vector<std::string> &out){
    
    //len is the length of the full request

    if (len < HEADER_LEN){
        printf("parse_req: message too short\n")
        return -1;
    }

    //Get the number of arguments
    uint32_t n = 0;
    memcpy(&n,data,HEADER_LEN);

    if (n > MSG_LEN){
        printf("parse_req: message too long\n");
        return -1;
    }

    size_t point = 4;


    //Get the args as strings into the vector
    while(n > 0){
        if (point + HEADER_LEN > len){
            printf("parse_req: message too long\n");
            return -1;
        }

        //Get the lengh of the argument
        uint32_t arg_len = 0;
        memcpy(&arg_len, &data[point], HEADER_LEN);

        if (arg_len + point + HEADER_LEN > len){
            printf("parse_req: message too long\n");
            return -1;
        }

        out.push_back(std::string(&data[point],arg_len));
        pos += HEADER_LEN + arg_len;
        n--;
    }

    if (pos != len){
        printf("parse_req: req badly formatted\n");
        return -1;
    }

    return 0;

}

int main(){
    
    int sv_fd = start_server();
    set_fd_nb(sv_fd);

    std::vector<Conn *> fd2conn;
    std::vector<struct pollfd> poll_args;

    while (true){

        poll_args.clear();

        //We put the servers socket to the first position of the vector
        struct pollfd pfd = {sv_fd,POLLIN,0};
        poll_args.push_back(pfd);


        //We put the clients socket to the next position of the vector
        for (Conn *conn: fd2conn){
            if (!conn){
                continue;
            }

            struct pollfd pfd = {};
            pfd.fd = conn->fd;
            if (conn->state==STATE_REQ){
                pfd.events = POLLIN;
            }
            else pfd.events = POLLOUT;

            pfd.events |= POLLERR;
            poll_args.push_back(pfd);
        }

        //Poll for active file descriptors
        int rv = poll(poll_args.data(), (nfds_t) poll_args.size(), TIMEOUT);

        if (rv < 0){
            die("poll");
        }

        //Process active connections
        for (size_t i = 1; i<poll_args.size(); ++i){

            //poll revents is a output filled by kernel with the events that happened
            if (poll_args[i].revents){
                Conn *conn = fd2conn[poll_args[i].fd];
                connection_io(conn);
                if (conn->state==STATE_END){
                    fd2conn[conn->fd] = NULL;
                    (void) close(conn->fd);
                    free(conn);
                }
            }
        }

        //If the server had any events, then someone tried to connect
        if (poll_args[0].revents) {
            accept_new_conn(fd2conn,sv_fd);
        }
    }

    return 0;

}