Скомпилировать символьный драйвер, создать спец файл, написать приложение для открытия спец файла

# Компиляция драйвера
```
ivan@ivan:~/drivers/pz3$ make
make -C /lib/modules/6.8.0-86-generic/build M=/home/ivan/drivers/pz3 modules
make[1]: Entering directory '/usr/src/linux-headers-6.8.0-86-generic'
warning: the compiler differs from the one used to build the kernel
  The kernel was built by: x86_64-linux-gnu-gcc-13 (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0
  You are using:           gcc-13 (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0
  CC [M]  /home/ivan/drivers/pz3/char_driver.o
  MODPOST /home/ivan/drivers/pz3/Module.symvers
  CC [M]  /home/ivan/drivers/pz3/char_driver.mod.o
  LD [M]  /home/ivan/drivers/pz3/char_driver.ko
  BTF [M] /home/ivan/drivers/pz3/char_driver.ko
Skipping BTF generation for /home/ivan/drivers/pz3/char_driver.ko due to unavailability of vmlinux
make[1]: Leaving directory '/usr/src/linux-headers-6.8.0-86-generic'
ivan@ivan:~/drivers/pz3$ ls
char_driver.c   char_driver.mod    char_driver.mod.o  Makefile       Module.symvers
char_driver.ko  char_driver.mod.c  char_driver.o      modules.order
```

# Вставка модуля
```
ivan@ivan:~/drivers/pz3$ sudo insmod char_driver.ko
[sudo] password for ivan:
ivan@ivan:~/drivers/pz3$ sudo dmesg | tail
[ 1464.645901] workqueue: drain_vmap_area_work hogged CPU for >10000us 4 times, consider switching to WQ_UNBOUND
[ 1471.749679] workqueue: e1000_watchdog [e1000] hogged CPU for >10000us 4 times, consider switching to WQ_UNBOUND
[ 1476.777996] hrtimer: interrupt took 17229936 ns
[ 1484.391237] workqueue: e1000_watchdog [e1000] hogged CPU for >10000us 8 times, consider switching to WQ_UNBOUND
[ 1484.888374] workqueue: drain_vmap_area_work hogged CPU for >10000us 8 times, consider switching to WQ_UNBOUND
[ 1485.357359] workqueue: drm_fb_helper_damage_work hogged CPU for >10000us 64 times, consider switching to WQ_UNBOUND
[ 1507.240243] workqueue: blk_mq_run_work_fn hogged CPU for >10000us 4 times, consider switching to WQ_UNBOUND
[ 1634.614708] char_driver: loading out-of-tree module taints kernel.
[ 1634.614720] char_driver: module verification failed: signature and/or required key missing - tainting kernel
[ 1634.622070] char_driver: loaded (major 240 minor 0)
```

# Создался файл устройства
```
ivan@ivan:~/drivers/pz3$ ls -l /dev/char_driver
crw------- 1 root root 240, 0 ноя 14 08:11 /dev/char_driver
```

# Скомпилировал simple_app
```
ivan@ivan:~/drivers/pz3$ gcc simple_app.c -o simple_app
ivan@ivan:~/drivers/pz3$ ls -l simple_app
-rwxrwxr-x 1 ivan ivan 16184 ноя 14 08:16 simple_app
```

# Запустил приложение
```
ivan@ivan:~/drivers/pz3$ sudo ./simple_app
Opening device: /dev/char_driver
Device responded (27 bytes): Message from char_driver!
```

# Выгрузил модуль
```
ivan@ivan:~/drivers/pz3$ lsmod | grep char_driver
char_driver            12288  0
ivan@ivan:~/drivers/pz3$ sudo rmmod char_driver
ivan@ivan:~/drivers/pz3$ sudo dmesg | tail
[ 1485.357359] workqueue: drm_fb_helper_damage_work hogged CPU for >10000us 64 times, consider switching to WQ_UNBOUND
[ 1507.240243] workqueue: blk_mq_run_work_fn hogged CPU for >10000us 4 times, consider switching to WQ_UNBOUND
[ 1634.614708] char_driver: loading out-of-tree module taints kernel.
[ 1634.614720] char_driver: module verification failed: signature and/or required key missing - tainting kernel
[ 1634.622070] char_driver: loaded (major 240 minor 0)
[ 1950.102386] workqueue: blk_mq_run_work_fn hogged CPU for >10000us 8 times, consider switching to WQ_UNBOUND
[ 1950.301221] workqueue: blk_mq_run_work_fn hogged CPU for >10000us 16 times, consider switching to WQ_UNBOUND
[ 1990.553598] char_driver: opened
[ 1990.553832] char_driver: closed
[ 2071.220479] char_driver: unloaded
```

1 `struct file_operations`
Назначение:
Связывает системные вызовы пользователя с функциями драйвера.

2 `dev_t Номер устройства`
Содержит:
major — номер драйвера
minor — номер устройства
Он используется ядром для определения:
какому драйверу принадлежит устройство
какое конкретно устройство внутри драйвера используется

3 `struct inode Описание файла в ядре`
Назначение:
Описывает файл устройства
Передаётся в open()

4 `struct file Открытый файл (экземпляр)`
Представляет открытое устройство
Хранит:
позицию чтения
приватные данные драйвера

Позволяет драйверу:
хранить состояние устройства
поддерживать несколько открытий
реализовывать буферы, очереди, контексты

5 `truct class Класс устройства (sysfs)`
Создаёт /sys/class/<name>/
Нужен для udev

6 `struct device Устройство в системе`
Назначение:
Представляет устройство в /sys
Создаёт /dev/char_driver

создаёт запись в sysfs
создаёт файл /dev/char_driver
связывает устройство с классом

7 `struct module Модуль ядра`
Назначение:
Предотвращает выгрузку модуля, пока устройство открыто
Без этого возможен краш ядра