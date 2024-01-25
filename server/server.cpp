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

    //Reply with echo
    memcpy(conn->wbuf, &len, HEADER_LEN);
    memcpy(conn->wbuf + HEADER_LEN, &conn->rbuf[HEADER_LEN], len);
    conn->wbuf_size = HEADER_LEN + len;

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

int main(){
    

    printf("Started\n");
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

        if (poll_args[0].revents) {
            accept_new_conn(fd2conn,sv_fd);
        }
    }

    return 0;

}