#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main() {
    char buf[100];
    int fd = open("/dev/zero", O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    ssize_t n = read(fd, buf, 100);
    if (n < 0) {
        perror("read");
        close(fd);
        return 1;
    }

    printf("Read %zd bytes\n", n);

    close(fd);
    return 0;
}
