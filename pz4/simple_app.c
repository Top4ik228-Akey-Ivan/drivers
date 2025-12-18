#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdbool.h>

#define DEV_PATH "/dev/char_driver"

#define IOCTL_CLEAR _IO('x', 1)
#define IOCTL_CHECK _IOR('x', 2, int*)

int main(void)
{
	int fd, empty = 0;
	char buffer[128];
	const char *msg = "Hello kernel!\n";

	printf("=== Testing char_driver ===\n\n");

	// Открываем файл устройства, Если драйвер не загружен или нет прав — будет ошибка
	fd = open(DEV_PATH, O_RDWR);
	if (fd < 0) {
		perror("open");
		return 1;
	}
	printf("Device opened\n\n");

	printf("Test 1: initial buffer state\n");

	// Отправляем ioctl команду в драйвер на проверку пустоты буфера
	if (ioctl(fd, IOCTL_CHECK, &empty) < 0) {
		perror("ioctl");
		return 1;
	}
	// 0 - полный 1-пустой
	printf("Buffer empty: %s\n\n", empty ? "yes" : "no");

	printf("Test 2: writing into driver\n");

	// user space → kernel space
	write(fd, msg, strlen(msg));
	printf("Wrote: %s\n\n", msg);

	printf("Test 3: reading from driver\n");
	// Очищаем буфер пользователя
	memset(buffer, 0, sizeof(buffer));
	// kernel space → user space
	read(fd, buffer, sizeof(buffer));
	printf("Read: %s\n\n", buffer);

	// Отправляем ioctl команду в драйвер на проверку пустоты буфера
	printf("Test 4: checking buffer after write\n");
	ioctl(fd, IOCTL_CHECK, &empty);
	printf("Buffer empty: %s\n\n", empty ? "yes" : "no");

	// Очистка буффера в драйвере
	printf("Test 5: clearing buffer\n");
	ioctl(fd, IOCTL_CLEAR);
	printf("Buffer cleared\n\n");

	// Отправляем ioctl команду в драйвер на проверку пустоты буфера
	printf("Test 6: checking after clear\n");
	ioctl(fd, IOCTL_CHECK, &empty);
	printf("Buffer empty: %s\n\n", empty ? "yes" : "no");

	close(fd);
	printf("=== Tests completed ===\n");

	return 0;
}
