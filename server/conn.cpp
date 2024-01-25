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

static bool try_read(Conn *conn){

    if (conn->rbuf_size > sizeof(conn->rbuf)){
        printf("try_read: rbuf_size > sizeof(conn->rbuf) SHOULD NOT HAPPEN\n");
        conn->state = STATE_END;
        return false;
    }

    ssize_t rv = 0;


    //Read from socket while there's info and errno is EINTR (interrupted system call)
    do {
        size_t cap = sizeof(conn->rbuf) - conn->rbuf_size;
        rv = read(conn->fd, &conn->rbuf[conn->rbuf_size], cap);
    } while ( rv < 0 && errno == EINTR);


    //If the socket has no more info it returns -1 and errno EAGAIN
    if (rv < 0){
        if (errno == EAGAIN) return false;
        printf("try_read: read error\n");
        conn->state = STATE_END;
        return false;
    }

    //If the read dind't read anything
    if (rv == 0){
        if (conn->rbuf_size > 0){
            printf("try_read: unexpected EOF\n");
        }
        else {
            printf("try_read: EOF\n");
        }
        conn->state = STATE_END;
        return false;
    }


    conn->rbuf_size += rv;

    if (conn->rbuf_size > sizeof(conn->rbuf)){
        printf("try_read: rbuf_size > sizeof(conn->rbuf) SHOULD NOT HAPPEN\n");
        conn->state = STATE_END;
        return false;
    }


    //Handle request is in a while loop because there can be multiple requests in the buffer
    //This mode of operation is called pipelining
    while(try_handle_request(conn)){}
    return (conn->state == STATE_REQ);
}

static bool try_write(Conn *conn){

    ssize_t rv = 0;

    //This does only one write call. If it gets EINTR, it got interrupted by an exception
    do {
        size_t remain = conn->wbuf_size - conn->wbuf_sent;
        rv = write(conn->fd, &conn->wbuf[conn->wbuf_sent], remain);
    } while (rv < 0 && errno == EINTR);

    if (rv<0){
        //When it gets EAGAIN there's nothing to write
        if (errno == EAGAIN) return false;
        printf("try_write: write error\n");
        conn->state = STATE_END;
        return false;
    }

    conn->wbuf_sent += (size_t) rv;

    if (conn->wbuf_sent > conn->wbuf_size){
        printf("try_read: wbuf_sent > wbuf_size SHOULD NOT HAPPEN\n");
        conn->state = STATE_END;
        return false;
    }

    //Response was fully sent, change state
    if (conn->wbuf_sent == conn->wbuf_size){
        conn->wbuf_sent = 0;
        conn->wbuf_size = 0;
        conn->state = STATE_REQ;
        return false;
    }

    //Wbuf still has some data, try to write it
    return true;
}

void state_req(Conn* conn){
    while(try_read(conn));
}


void state_res(Conn* conn){
    while(try_write(conn));
}

//State machine for a connection
void connection_io(Conn *conn){
    if (conn->state == STATE_REQ){
        state_req(conn);
    }
    else if (conn->state == STATE_RES){
        state_res(conn);
    }
    else {
        printf("Error: invalid state\n");
        conn->state = STATE_END;
    }
}