NET_BUILD := net
.DEFAULT_GOAL := $(NET_BUILD)
CC = gcc
CFLAGS += -g
CFLAGS += -DDEBUG_MODE
LDFLAGS +=  -L$(shell pwd)/../platform//lib/ -lconfig
LDFLAGS += -lpcap
LDFLAGS += -lpthread
INC_PATH = $(shell pwd)/include/ $(shell pwd)/include/generated $(shell pwd)/include/pcap
INC_PATH += $(shell pwd)/../platform/include/util
INC_PATH += $(shell pwd)/../platform/include/system
INC_PATH += $(shell pwd)/../platform/include/safety
INC_PATH += $(shell pwd)/../platform/init  
INC_FILE = $(addprefix -I,$(INC_PATH))
BIN_DIR = $(shell pwd)/bin
NET_SRC_PATH = $(shell pwd)/src
NET_SRC_LIST = $(wildcard $(NET_SRC_PATH)/*.c)
NET_SRC_FILE = $(notdir $(NET_SRC_LIST))
NET_OBJ_LIST = $(patsubst %.c, $(BIN_DIR)/%.o, $(NET_SRC_FILE))

$(BIN_DIR)/%.o: $(NET_SRC_PATH)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@ $(INC_FILE)
	@echo +CC $<	

$(NET_BUILD): $(NET_OBJ_LIST)
	@$(CC) -o $(BIN_DIR)/$@ $^ $(LDFLAGS)
	@echo LD -o $@

clean:
	@rm $(BIN_DIR)/*