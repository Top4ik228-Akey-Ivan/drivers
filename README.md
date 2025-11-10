# Task_1

`makefile`
Файл для компиляции с поиощью make
```
CC = gcc
CFLAGS = -Wall -Wextra -O2

TARGET = read_devices

all: $(TARGET)

$(TARGET): read_devices.c
	$(CC) $(CFLAGS) -o $(TARGET) read_devices.c

clean:
	rm -f $(TARGET)

```

`read_devices.c` 
Файл для считывания данных
```
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
```

# task_2
`Вывод утилиты strace`
```
ivan@ivan:~/drivers/pz1/task1$ strace ./read_devices
execve("./read_devices", ["./read_devices"], 0x7ffcb11d21d0 /* 23 vars */) = 0
brk(NULL)                               = 0x18987000
brk(0x18987d00)                         = 0x18987d00
arch_prctl(ARCH_SET_FS, 0x18987380)     = 0
set_tid_address(0x18987650)             = 2259
set_robust_list(0x18987660, 24)         = 0
rseq(0x18987ca0, 0x20, 0, 0x53053053)   = 0
prlimit64(0, RLIMIT_STACK, NULL, {rlim_cur=8192*1024, rlim_max=RLIM64_INFINITY}) = 0
readlinkat(AT_FDCWD, "/proc/self/exe", "/home/ivan/drivers/pz1/task1/rea"..., 4096) = 41
getrandom("\x0f\x73\x20\x90\xb9\xe0\x31\xaf", 8, GRND_NONBLOCK) = 8
brk(NULL)                               = 0x18987d00
brk(0x189a8d00)                         = 0x189a8d00
brk(0x189a9000)                         = 0x189a9000
mprotect(0x4ad000, 20480, PROT_READ)    = 0
openat(AT_FDCWD, "/dev/zero", O_RDONLY) = 3
read(3, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"..., 100) = 100
fstat(1, {st_mode=S_IFCHR|0620, st_rdev=makedev(0x88, 0), ...}) = 0
write(1, "Read 100 bytes\n", 15Read 100 bytes
)        = 15
close(3)                                = 0
exit_group(0)                           = ?
+++ exited with 0 +++
ivan@ivan:~/drivers/pz1/task1$
```