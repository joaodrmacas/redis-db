#include "conn.hpp"

//Put a connection in the connections vector
void conn_put(std::vector<Conn *> &fd2conn, Conn *conn){

    if (conn->fd < 0) {
        printf("Error: conn_put called with invalid fd\n");
        return;
    }

    if (fd2conn.size() <= (size_t) conn->fd){
        fd2conn.resize((size_t) conn->fd + 1);
    }
    fd2conn[conn->fd] = conn;
}

//Handle a request from a client
int32_t accept_new_conn(std::vector<Conn *> &fd2conn, int fd){  

    struct sockaddr_in client_addr;
    socklen_t socklen = sizeof(client_addr);

    int connfd = accept(fd, (struct sockaddr *) &client_addr, &socklen);

    if (connfd < 0){
        printf("accept error\n");
        return -1;
    }

    set_fd_nb(connfd);

    struct Conn *conn = (struct Conn *) malloc(sizeof(struct Conn));
    
    if (!conn){
        close(connfd);
        return -1;
    }

    conn->fd = connfd;
    conn->state = STATE_REQ;
    conn->rbuf_size = 0;
    conn->wbuf_size = 0;
    conn->wbuf_sent = 0;

    conn_put(fd2conn, conn);
    return 0;
}

void state_req(Conn* conn){
    return;
}

void state_res(Conn* conn){
    return;
}

//State machine for a connection
void connection_io(Conn *conn){
    if (conn->state == STATE_REQ){
        //state_req(conn);
    }
    else if (conn->state == STATE_RES){
        //state_res(conn);
    }
    else {
        printf("Error: invalid state\n");
        conn->state = STATE_END;
    }
}