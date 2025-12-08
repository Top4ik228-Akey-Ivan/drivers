#pragma once

#include <linux/ioctl.h>

// Магическое число для ioctl-команд
#define MAI_MAGIC_NUM 0x11

// Получить длину буфера гистограммы
#define IOCTL_HISTO_LEN _IOR(MAI_MAGIC_NUM, 0, size_t)

// Получить данные гистограммы
#define IOCTL_HISTO_BUF _IOR(MAI_MAGIC_NUM, 1, size_t *)
