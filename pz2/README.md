# Модуль ядра, скомпилировал
```
ivan@ivan:~/drivers/pz2$ make
make -C /lib/modules/6.8.0-86-generic/build/ M=/home/ivan/drivers/pz2 modules
make[1]: Entering directory '/usr/src/linux-headers-6.8.0-86-generic'
warning: the compiler differs from the one used to build the kernel
  The kernel was built by: x86_64-linux-gnu-gcc-13 (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0
  You are using:           gcc-13 (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0
  CC [M]  /home/ivan/drivers/pz2/simple.o
  MODPOST /home/ivan/drivers/pz2/Module.symvers
  CC [M]  /home/ivan/drivers/pz2/simple.mod.o
  LD [M]  /home/ivan/drivers/pz2/simple.ko
  BTF [M] /home/ivan/drivers/pz2/simple.ko
Skipping BTF generation for /home/ivan/drivers/pz2/simple.ko due to unavailability of vmlinux
make[1]: Leaving directory '/usr/src/linux-headers-6.8.0-86-generic'
```

# Insmod (lsmod\dmesg)
```
ivan@ivan:~/drivers/pz2$ lsmod | grep simple
simple                 12288  0

ivan@ivan:~/drivers/pz2$ sudo dmesg | grep Hello
[sudo] password for ivan:
[ 7299.483627] Hello from the mai 307 team
```

# Удалить модуль ядра
```
ivan@ivan:~/drivers/pz2$ sudo rmmod simple.ko
[sudo] password for ivan:
ivan@ivan:~/drivers/pz2$ lsmod | grep simple
ivan@ivan:~/drivers/pz2$ nano dmesg | grep Goodbye
[747654.794831] Goodbye from the mai 307 team!
```