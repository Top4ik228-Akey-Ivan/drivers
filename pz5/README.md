Пользовательское приложение (user-space) — отправляет сырые Ethernet-кадры через сетевой интерфейс.

Простейший сетевой драйвер ядра Linux (kernel-space) — принимает эти кадры и выводит их содержимое в dmesg.

# Компиляция драйвера
```
ivan@ivan:~/drivers/pz5$ make
make -C /lib/modules/6.8.0-87-generic/build M=/home/ivan/drivers/pz5 modules
make[1]: Entering directory '/usr/src/linux-headers-6.8.0-87-generic'
warning: the compiler differs from the one used to build the kernel
  The kernel was built by: x86_64-linux-gnu-gcc-13 (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0
  You are using:           gcc-13 (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0
  CC [M]  /home/ivan/drivers/pz5/ethernet_driver.o
  MODPOST /home/ivan/drivers/pz5/Module.symvers
  CC [M]  /home/ivan/drivers/pz5/ethernet_driver.mod.o
  LD [M]  /home/ivan/drivers/pz5/ethernet_driver.ko
  BTF [M] /home/ivan/drivers/pz5/ethernet_driver.ko
Skipping BTF generation for /home/ivan/drivers/pz5/ethernet_driver.ko due to unavailability of vmlinux
make[1]: Leaving directory '/usr/src/linux-headers-6.8.0-87-generic'
```
# Загрузка модуля ядра
```
ivan@ivan:~/drivers/pz5$ sudo insmod ethernet_driver.ko
[sudo] password for ivan:
```

# dmesg
```
ivan@ivan:~/drivers/pz5$ sudo dmesg | tail
[ 4547.095744]  x86_64_start_kernel+0xbf/0x110
[ 4547.095747]  secondary_startup_64_no_verify+0x184/0x18b
[ 4547.095761]  </TASK>
[ 4625.168415] e1000: enp0s3 NIC Link is Down
[ 4629.263019] e1000: enp0s3 NIC Link is Up 1000 Mbps Full Duplex, Flow Control: RX
[ 4752.622775] workqueue: blk_mq_run_work_fn hogged CPU for >10000us 16 times, consider switching to WQ_UNBOUND
[ 5439.061511] e1000: enp0s3 NIC Link is Down
[ 5443.167893] e1000: enp0s3 NIC Link is Up 1000 Mbps Full Duplex, Flow Control: RX
[ 6277.493646] rawdemo_driver: driver loaded
[ 6421.331579] workqueue: blk_mq_run_work_fn hogged CPU for >10000us 32 times, consider switching to WQ_UNBOUND
```

# Поднять интерфейс драйвера
```
ivan@ivan:~/drivers/pz5$ ip link
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN mode DEFAULT group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
2: enp0s3: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UP mode DEFAULT group default qlen 1000
    link/ether 08:00:27:c2:d2:f5 brd ff:ff:ff:ff:ff:ff
3: eth0: <BROADCAST,MULTICAST> mtu 1500 qdisc noop state DOWN mode DEFAULT group default qlen 1000
    link/ether 00:00:00:00:00:00 brd ff:ff:ff:ff:ff:ff
ivan@ivan:~/drivers/pz5$ sudo ip link set eth0 up
```

# Компиляция и запук приложения
```
ivan@ivan:~/drivers/pz5$ gcc ethernet_sender.c -o sender
ivan@ivan:~/drivers/pz5$ sudo ./sender
Frame sent successfully
```

# dmesg
```
[ 7458.769553] rawdemo_driver: transmit invoked
[ 7458.769564] rawdemo_driver: skb length=25
[ 7458.769568] rawdemo_driver: RAW HEX: 00 00 00 00 00 00 00 00 00 00 00 00 12 34 68 65 6c 6c 6f 20 77 6f 72 6c 64
[ 7458.769594] rawdemo_driver: ASCII: \x00 \x00 \x00 \x00 \x00 \x00 \x00 \x00 \x00 \x00 \x00 \x00 \x12 4 h e l l o   w o r l d
```