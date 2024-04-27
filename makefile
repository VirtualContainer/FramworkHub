WORKDIR := $(shell pwd)
CONFIG_DIR = $(WORKDIR)/platform
NET_DIR = $(WORKDIR)/net

CONFIG_MODULE := platform
NET_MODULE := net

#构建所有模块
Train: $(CONFIG_MODULE) $(NET_MODULE)
	make -C $(CONFIG_DIR)
	make -C $(NET_DIR)

menuconfig:
	make menuconfig -C $(NET_DIR)

clean:
	make clean -C $(NET_DIR)
	make clean -C $(CONFIG_DIR)