# Задание
Собрать конфигурацию buildroot

## Склониировал buildroot и сделал checkout
```
git clone https://git.buildroot.net/buildroot
git checkout 2025.02.8
```
## Запустил процесс сборки
```
make beaglebone_defconfig
make menuconfig
```

## Изменил натсройки конфигурации buildroot
`Buildroot toolchain" -> "External toolchain`

`Root password: "admin"`

`Custom tarball" -> "Latest Version (6.12)`

`bb.org" -> "omap2plus`

## Запустил процесс окнфигурации ядра
```
make linux-menuconfig
```

## Скомпилировал
```
make
```
## Посмотрел файлы после компиляции
```
ivan@ivan:~/drivers/lab2/buildroot$ ls output/images
am335x-boneblack.dtb           am335x-bone.dtb                am335x-evm.dtb    extlinux     rootfs.ext4  u-boot.img
am335x-boneblack-wireless.dtb  am335x-bonegreen.dtb           am335x-evmsk.dtb  MLO          rootfs.tar   uEnv.txt
am335x-boneblue.dtb            am335x-bonegreen-wireless.dtb  boot.vfat         rootfs.ext2  sdcard.img   zImage
```