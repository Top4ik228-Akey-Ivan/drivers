#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DEV_PATH "/dev/char_driver"

int main(void)
{
    int fd;
    char buffer[128];
    ssize_t bytes_read;

    printf("Opening device: %s\n", DEV_PATH);

    fd = open(DEV_PATH, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open device");
        return 1;
    }

    bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("Failed to read");
        close(fd);
        return 1;
    }

    buffer[bytes_read] = '\0';  // гарантируем строку

    printf("Device responded (%zd bytes): %s", bytes_read, buffer);

    close(fd);
    return 0;
}
