KCONFIG_PATH := $(shell pwd)/tools
KCONFIG_TOOL = $(KCONFIG_PATH)/mconf
KCONFIG_CONFIG = .config.autoconf
Kconfig := $(shell pwd)/Kconfig

menuconfig:
	$(KCONFIG_TOOL) $(Kconfig)