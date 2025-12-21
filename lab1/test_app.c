#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "ioctl.h"

// Чтение одного значения из устройства
static void dev_read_single(const int fd)
{
    int value = 0;
    ssize_t read_len = read(fd, &value, sizeof(value));

    if (read_len < 0) {
        printf("dev_read_single: read failed: %ld\n", read_len);
        return;
    }

    printf("dev_read_single: %d\n", value);
}

// Запись одного значения в устройство
static void dev_write_single(const int fd, const int value)
{
    ssize_t wrote_len = write(fd, &value, sizeof(value));

    if (wrote_len < 0) {
        printf("dev_write_single: write failed: %ld\n", wrote_len);
        return;
    }

    printf("dev_write_single: %d\n", value);
}

int main()
{
    const char dev_path[] = "/dev/mai_lab1_dev";

    // открываем устройство только для чтения / Только читает
    // так проще моделировать независимые операции
    int fd_r = open(dev_path, O_RDONLY);
    if (fd_r < 0) {
        printf("Failed to open %s for reading: %d\n", dev_path, fd_r);
        return fd_r;
    }

    // открываем устройство только для записи / Только пишет
    int fd_w = open(dev_path, O_WRONLY);
    if (fd_w < 0) {
        printf("Failed to open %s for writing: %d\n", dev_path, fd_w);
        close(fd_r);
        return fd_w;
    }

    // Генерация нагрузки
    for (size_t i = 0; i < 1000; i++) {
        dev_write_single(fd_w, i);
        dev_read_single(fd_r);
    }

    size_t histo_len = 0;
    int ioctl_err = 0;

    // Получение длины гистограммы
    if ((ioctl_err = ioctl(fd_r, IOCTL_HISTO_LEN, &histo_len))) {
        printf("Failed to ioctl IOCTL_HISTO_LEN: %d\n", ioctl_err);
        return -1;
    }

    printf("Histogram length: %zu\n", histo_len);

    // Получение данных гистограммы
    // динамический массив под данные от драйвера => Если памяти нет — аварийный выход
    size_t *histo_buf = malloc(histo_len * sizeof(size_t));
    if (!histo_buf) {
        printf("Failed to allocate memory for histogram\n");
        return -1;
    }

    // Получение данных гистограммы
    if ((ioctl_err = ioctl(fd_r, IOCTL_HISTO_BUF, histo_buf))) {
        printf("Failed to ioctl IOCTL_HISTO_BUF: %d\n", ioctl_err);
        free(histo_buf);
        return -1;
    }

    for (size_t i = 0; i < histo_len; i++) {
        printf("%zu:\t%zu\n", i, histo_buf[i]);
    }

    // освобождение памяти => закрытие файлов
    free(histo_buf);
    close(fd_r);
    close(fd_w);

    return 0;
}
