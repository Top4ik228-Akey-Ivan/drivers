Написать PCI драйвер с probe и release для сетевого адаптера.
Вставить в probe инициализацию сетевого драйвера с предидущего занятия.
Реализовать чтение MAC адреса
# Сборка 
```
ivan@ivan:~/drivers/pz6$ make
make -C /lib/modules/6.8.0-87-generic/build M=/home/ivan/drivers/pz6 modules
make[1]: Entering directory '/usr/src/linux-headers-6.8.0-87-generic'
  CC [M]  /home/ivan/drivers/pz6/pcidemo.o
  LD [M]  /home/ivan/drivers/pz6/pcidemo.ko
make[1]: Leaving directory '/usr/src/linux-headers-6.8.0-87-generic'

```

# Отключение физического драйвера
```
ivan@ivan:~/drivers/pz6$ sudo ip link set enp0s3 down
ivan@ivan:~/drivers/pz6$ sudo rmmod e1000
```
# Вставка модуля
```
ivan@ivan:~/drivers/pz6$ sudo insmod pcidemo.ko
ivan@ivan:~/drivers/pz6$ dmesg | tail -n 20
[12516.408884] pci_eth_demo: PCI probe started
[12516.408900] pci_eth_demo: MAC read from PCI config: 08:00:27:c2:d2:f5
[12516.408920] pci_eth_demo: PCI probe OK, virtual NIC ready
[12516.409100] pci_eth_demo: netdev open
```

# Проверка интерфейса
```
ivan@ivan:~/drivers/pz6$ ip link show
2: eth0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UP mode DEFAULT group default qlen 1000
link/ether 08:00:27:c2:d2:f5 brd ff:ff:ff:ff:ff:ff
```

# Сборка и запуск приложения
```
ivan@ivan:~/drivers/pz6$ make pcidemo_app
gcc -o pcidemo_app pcidemo_app.c
ivan@ivan:~/drivers/pz6$ sudo ./pcidemo_app
Frame sent successfully
```

# Проверка логов и запуск приложения
```
ivan@ivan:~/drivers/pz6$ dmesg | tail -n 20
[12516.410215] pci_eth_demo: transmit invoked, skb=25 bytes
[12516.410218] pci_eth_demo: RAW HEX: 00 12 34 56 78 90 08 00 27 c2 d2 f5 12 34 68 65 6c 6c 6f 20 77 6f 72 6c 64
[12516.410220] pci_eth_demo: ASCII:  ␀  ␁ 4 5 6  x  ␈  ␀  '  ␂  ␃  ␄  1 2 h e l l o   w o r l d
```

# Выгрузка модуля
```
ivan@ivan:~/drivers/pz6$ sudo ip link set eth0 down
ivan@ivan:~/drivers/pz6$ sudo rmmod pcidemo
ivan@ivan:~/drivers/pz6$ dmesg | tail -n 10
[12516.512345] pci_eth_demo: PCI device removed

```