obj-m := pcidemo.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all: pcidemo_app pcidemo.ko

pcidemo.ko:
	make -C $(KDIR) M=$(PWD) modules

pcidemo_app: pcidemo_app.c
	gcc -o pcidemo_app pcidemo_app.c

clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -f pcidemo_app
