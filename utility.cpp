#include "utility.hpp"

int32_t read_full(int fd,char* buf, size_t n){
    while (n > 0){
        ssize_t rv = read(fd, buf, n);
        if (rv<=0) return -1;

        assert( (size_t) rv <= n);

        n -= (size_t) rv;
        buf += rv;
    }

    return 0;
}

int32_t write_full(int fd, const char* buf, size_t n){
    while (n > 0){
        ssize_t rv = write(fd, buf, n);
        if (rv<=0) return -1;

        assert( (size_t) rv <= n);

        n -= (size_t) rv;
        buf += rv;
    }

    return 0;
}


void die(const char *s) {
    perror(s);
    exit(EXIT_FAILURE);
}

void set_fd_nb(int fd){
    errno = 0;
    int flags = fcntl(fd,F_GETFL,0);
    if (errno){
        die("fcntl");
        return;
    }

    flags |= O_NONBLOCK;

    errno = 0;
    fcntl(fd,F_SETFL,flags);
    if (errno){
        die("fcntl");
    }
}