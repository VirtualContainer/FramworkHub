WORKDIR := $(shell pwd)


#CONFIG的构建
CONFIG_MODULE = $(WORKDIR)/bin/config
CC = gcc
LDFLAGS += -lpthread 
CFLAGS += -g
CFLAGS += -DDEBUG_MODE
CFLAGS += -D INTEGERTYPE=0
CFLAGS += -D FLOATINGTYPE=1
CFLAGS += -D MATRIX_STORETYPE=INTEGERTYPE
MODULE0 := util
MODULE1 := system
MODULE2 := safety

INC_PATH := $(WORKDIR)/init $(WORKDIR)/include/$(MODULE0) $(WORKDIR)/include/$(MODULE1) $(WORKDIR)/include/$(MODULE2) 
INC_FILE = $(addprefix -I,$(INC_PATH))

MAIN_SRC_PATH = $(shell pwd)/init
UTIL_SRC_PATH = $(shell pwd)/src/$(MODULE0)
SYSTEM_SRC_PATH = $(shell pwd)/src/$(MODULE1)
SAFETY_SRC_PATH = $(shell pwd)/src/$(MODULE2)
MAIN_SRC_LIST = $(wildcard $(MAIN_SRC_PATH)/*.c)
UTIL_SRC_LIST = $(wildcard $(UTIL_SRC_PATH)/*.c)
SYSTEM_SRC_LIST = $(wildcard $(SYSTEM_SRC_PATH)/*.c)
SAFETY_SRC_LIST = $(wildcard $(SAFETY_SRC_PATH)/*.c)
MAIN_SRC_FILE = $(notdir $(MAIN_SRC_LIST))
UTIL_SRC_FILE = $(notdir $(UTIL_SRC_LIST))
SYSTEM_SRC_FILE = $(notdir $(SYSTEM_SRC_LIST))
SAFETY_SRC_FILE = $(notdir $(SAFETY_SRC_LIST))

BIN_DIR := $(shell pwd)/bin
MAIN_BIN_DIR = $(BIN_DIR)/main
UTIL_BIN_DIR = $(BIN_DIR)/util
SYSTEM_BIN_DIR = $(BIN_DIR)/system
SAFETY_BIN_DIR = $(BIN_DIR)/safety
BIN_PATH = $(shell pwd)/bin

MAIN_OBJ_LIST = $(patsubst %.c, $(MAIN_BIN_DIR)/%.o, $(MAIN_SRC_FILE))
UTIL_OBJ_LIST = $(patsubst %.c, $(UTIL_BIN_DIR)/%.o, $(UTIL_SRC_FILE))
SYSTEM_OBJ_LIST =  $(patsubst %.c, $(SYSTEM_BIN_DIR)/%.o, $(SYSTEM_SRC_FILE))
SAFETY_OBJ_LIST =  $(patsubst %.c, $(SAFETY_BIN_DIR)/%.o, $(SAFETY_SRC_FILE))


$(UTIL_BIN_DIR)/%.o: $(UTIL_SRC_PATH)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@ $(INC_FILE)  
	@echo +CC $<
$(SYSTEM_BIN_DIR)/%.o: $(SYSTEM_SRC_PATH)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@ $(INC_FILE)
	@echo +CC $<
$(SAFETY_BIN_DIR)/%.o: $(SAFETY_SRC_PATH)/%.c
	@$(CC) -c $< -o $@ $(INC_FILE)
	@echo +CC $<
$(MAIN_BIN_DIR)/%.o: $(MAIN_SRC_PATH)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@ $(INC_FILE)
	@echo +CC $<	  
$(CONFIG_MODULE): $(MAIN_OBJ_LIST) $(SYSTEM_OBJ_LIST) $(UTIL_OBJ_LIST) $(SAFETY_OBJ_LIST)
	@$(CC) -o $@ $^ $(LDFLAGS)
	@echo LD -o $@


list:
	@echo "Dir: $(shell pwd)"


LIB_DIR = $(shell pwd)/lib
UTIL_SHARE_DIR = $(LIB_DIR)/$(MODULE0)
SYSTEM_SHARE_DIR = $(LIB_DIR)/$(MODULE1)
SAFETY_SHARE_DIR = $(LIB_DIR)/$(MODULE2)
INIT_SHARE_DIR = $(LIB_DIR)/init

build:
	@echo "the project is built..."
	@mkdir -p $(MAIN_BIN_DIR)
	@mkdir -p $(UTIL_BIN_DIR)
	@mkdir -p $(SYSTEM_BIN_DIR)
	@mkdir -p $(SAFETY_BIN_DIR)
	@mkdir -p $(UTIL_SHARE_DIR)
	@mkdir -p $(SYSTEM_SHARE_DIR)
	@mkdir -p $(SAFETY_SHARE_DIR)
	@mkdir -p $(INIT_SHARE_DIR)

UTIL_SHARE_LIST = $(patsubst %.c, $(UTIL_SHARE_DIR)/%.o, $(UTIL_SRC_FILE))
SYSTEM_SHARE_LIST =  $(patsubst %.c, $(SYSTEM_SHARE_DIR)/%.o, $(SYSTEM_SRC_FILE))
SAFETY_SHARE_LIST =  $(patsubst %.c, $(SAFETY_SHARE_DIR)/%.o, $(SAFETY_SRC_FILE))
INIT_SHARE_LIST = $(patsubst %.c, $(INIT_SHARE_DIR)/%.o, $(MAIN_SRC_FILE))
SHARE_CFLAGS = -Wextra -fPIC
SHARE_CFLAGS += -DDEBUG_MODE
SHARE_CFLAGS += -D INTEGERTYPE=0
SHARE_CFLAGS += -D FLOATINGTYPE=1
SHARE_CFLAGS += -D MATRIX_STORETYPE=INTEGERTYPE
SHARE_LDFLAGS = -shared -lpthread

$(UTIL_SHARE_DIR)/%.o: $(UTIL_SRC_PATH)/%.c
	@$(CC) $(SHARE_CFLAGS) -c $< -o $@ $(INC_FILE)  
	@echo +CC $<
$(SYSTEM_SHARE_DIR)/%.o: $(SYSTEM_SRC_PATH)/%.c
	@$(CC) $(SHARE_CFLAGS) -c $< -o $@ $(INC_FILE)
	@echo +CC $<
$(SAFETY_SHARE_DIR)/%.o: $(SAFETY_SRC_PATH)/%.c
	@$(CC) $(SHARE_CFLAGS) -c $< -o $@ $(INC_FILE)  
	@echo +CC $<
$(INIT_SHARE_DIR)/%.o: $(MAIN_SRC_PATH)/%.c
	@$(CC) $(SHARE_CFLAGS) -c $< -o $@ $(INC_FILE)  
	@echo +CC $<
share: $(SYSTEM_SHARE_LIST) $(UTIL_SHARE_LIST) $(SAFETY_SHARE_LIST) $(INIT_SHARE_LIST)
	@$(CC) $(SHARE_LDFLAGS) -o $(LIB_DIR)/libconfig.so $^
	@sudo mv $(LIB_DIR)/libconfig.so /usr/lib/
clean:
	@rm $(MAIN_BIN_DIR)/*.o $(UTIL_BIN_DIR)/*.o $(SYSTEM_BIN_DIR)/*.o $(SAFETY_BIN_DIR)/*.o
