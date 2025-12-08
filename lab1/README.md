# Задание
Написать драйвер с функциями open/read/write/close/ioctl
read будет читать int из буфера и сторить гистограмму/write будет писать в буфер/ioctl будет получать размер буфера и сам буфер

## Скомпилировать драйвер и тестовое приложение
```
ivan@ivan:~/drivers/lab1$ make
make -C /lib/modules/6.8.0-87-generic/build M=/home/ivan/drivers/lab1 modules
make[1]: Entering directory '/usr/src/linux-headers-6.8.0-87-generic'
warning: the compiler differs from the one used to build the kernel
  The kernel was built by: x86_64-linux-gnu-gcc-13 (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0
  You are using:           gcc-13 (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0
make[1]: Leaving directory '/usr/src/linux-headers-6.8.0-87-generic'
cc -Wall test_app.c -o test_app
```
## Загрузить ядро
```
ivan@ivan:~/drivers/lab1$ sudo insmod my_driver.ko
```
## Посмотреть логи
```
ivan@ivan:~/drivers/lab1$ sudo dmesg | tail
[ 4196.360441] mai_lab1_driver: device_ioctl: cmd=0x80081100
[ 4196.360444] mai_lab1_driver: device_ioctl: DRIVER_GET_HISTOGRAM_LEN
[ 4196.360449] mai_lab1_driver: device_ioctl: cmd=0x80081101
[ 4196.360451] mai_lab1_driver: device_ioctl: DRIVER_GET_HISTOGRAM_BUF
[ 4196.360658] mai_lab1_driver: device_release: trying to release the device
[ 4196.360662] mai_lab1_driver: device_release: trying to release the device
[ 4655.361001] mai_lab1_driver: cleaning up driver
[ 4655.362728] mai_lab1_driver: driver unloaded successfully
[ 4669.133824] mai_lab1_driver: trying to initialize the driver
[ 4669.135057] mai_lab1_driver: successfully initialized the driver
```

## Запустить тестовое приложение
```
ivan@ivan:~/drivers/lab1$ sudo ./test_app
dev_write_single: 0
dev_read_single: 0
dev_write_single: 1
dev_read_single: 1
dev_write_single: 2
dev_read_single: 2
dev_write_single: 3
dev_read_single: 3
-
-
-
dev_write_single: 997
dev_read_single: 997
dev_write_single: 998
dev_read_single: 998
dev_write_single: 999
dev_read_single: 999
Histogram length: 7
0:      229
1:      3
2:      74
3:      436
4:      87
5:      13
6:      25
```