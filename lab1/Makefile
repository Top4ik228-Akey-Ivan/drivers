# =============================
#      CONFIGURATION
# =============================

KDIR := /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

DRIVER_OBJ := my_driver
APP_NAME   := test_app

# =============================
#       MAIN TARGETS
# =============================

all: driver app

# ---- Build kernel module ----
driver:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# ---- Build user app ----
app:
	$(CC) -Wall test_app.c -o $(APP_NAME)

# ---- Clean everything ----
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	$(RM) $(APP_NAME)

# =============================
#   LOAD / UNLOAD CONVENIENCE
# =============================

load:
	sudo insmod $(DRIVER_OBJ).ko

unload:
	sudo rmmod $(DRIVER_OBJ)

reload: unload load

# =============================
#        PHONY TARGETS
# =============================
.PHONY: all driver app clean load unload reload
