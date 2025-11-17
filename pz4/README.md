Реализовать символьный драйвер в буфером 20 байт, Базовые операции (чтение, запись) и две команды для управления

# Компиляция драйвера
```
ivan@ivan:~/drivers/pz4$ make
make -C /lib/modules/6.8.0-87-generic/build M=/home/ivan/drivers/pz4 modules
make[1]: Entering directory '/usr/src/linux-headers-6.8.0-87-generic'
warning: the compiler differs from the one used to build the kernel
  The kernel was built by: x86_64-linux-gnu-gcc-13 (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0
  You are using:           gcc-13 (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0
  CC [M]  /home/ivan/drivers/pz4/char_driver.o
  MODPOST /home/ivan/drivers/pz4/Module.symvers
  CC [M]  /home/ivan/drivers/pz4/char_driver.mod.o
  LD [M]  /home/ivan/drivers/pz4/char_driver.ko
  BTF [M] /home/ivan/drivers/pz4/char_driver.ko
Skipping BTF generation for /home/ivan/drivers/pz4/char_driver.ko due to unavailability of vmlinux
make[1]: Leaving directory '/usr/src/linux-headers-6.8.0-87-generic'
```

# Загрузка драйвера
```
ivan@ivan:~/drivers/pz4$ sudo insmod char_driver.ko
[sudo] password for ivan:
ivan@ivan:~/drivers/pz4$ lsmod | grep char_driver
char_driver            12288  0
ivan@ivan:~/drivers/pz4$ sudo dmesg | tail
[   15.815422] workqueue: drm_fb_helper_damage_work hogged CPU for >10000us 32 times, consider switching to WQ_UNBOUND
[   15.892731] loop0: detected capacity change from 0 to 8
[   26.974097] systemd-journald[316]: File /var/log/journal/4ed6db7c4ec143a296980474d7cde963/user-1000.journal corrupted or uncleanly shut down, renaming and replacing.
[  152.063375] workqueue: drain_vmap_area_work hogged CPU for >10000us 4 times, consider switching to WQ_UNBOUND
[  194.274971] workqueue: drain_vmap_area_work hogged CPU for >10000us 8 times, consider switching to WQ_UNBOUND
[  514.973156] workqueue: drain_vmap_area_work hogged CPU for >10000us 16 times, consider switching to WQ_UNBOUND
[  812.200893] workqueue: drain_vmap_area_work hogged CPU for >10000us 32 times, consider switching to WQ_UNBOUND
[ 1585.587498] char_driver: loading out-of-tree module taints kernel.
[ 1585.587508] char_driver: module verification failed: signature and/or required key missing - tainting kernel
[ 1585.594792] char_driver: loaded major=240 minor=0
```

# Компиляция И запуск приложения
```
ivan@ivan:~/drivers/pz4$ gcc -o simple_app simple_app.c
ivan@ivan:~/drivers/pz4$ sudo ./simple_app
=== Testing char_driver ===

Device opened

Test 1: initial buffer state
Buffer empty: yes

Test 2: writing into driver
Wrote: Hello kernel!


Test 3: reading from driver
Read: Hello kernel!


Test 4: checking buffer after write
Buffer empty: no

Test 5: clearing buffer
Buffer cleared

Test 6: checking after clear
Buffer empty: yes

=== Tests completed ===
```