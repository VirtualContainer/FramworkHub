PROJECT = train
CC = gcc
CFLAGS += -DDEBUG_MODE
CFLAGS += -D INTEGERTYPE=0
CFLAGS += -D FLOATINGTYPE=1
CFLAGS += -D MATRIX_STORETYPE=INTEGERTYPE
MODULE0 := base
MODULE1 := system
MODULE2 := safety
BASEINCLUDEPATH := ./include/$(MODULE0)
SYSTEMINCLUDEPATH := ./include/$(MODULE1)
SAFETYINCLUDEPATH := ./include/$(MODULE2)
INCLUDEPATH = $(BASEINCLUDEPATH) $(SYSTEMINCLUDEPATH) $(SAFEINCLUDEPATH) 
MAINSRCPATH = .
BASESRCPATH = ./src/$(MODULE0)
SYSTEMSRCPATH = ./src/$(MODULE1)
SAFETYSRCPATH = ./src/$(MODULE2)
SRCLIST += $(wildcard $(MAINSRCPATH)/*.c)
SRCLIST += $(wildcard $(BASESRCPATH)/*.c)
SRCLIST += $(wildcard $(SYSTEMSRCPATH)/*.c)
SRCLIST += $(wildcard $(SAFETYSRCPATH)/*.c)
SRCFILE = $(notdir $(SRCLIST))
OBJECTLIST = $(SRCFILE:.c=.o)

LIBDIR = ./lib
BINDIR = ./bin
BINPATH = $(pwd)/bin/

%.o:$(MAINSRCPATH)/%.c
	$(CC) $(CFLAGS) -c $< -I$(SYSTEMINCLUDEPATH) -I$(BASEINCLUDEPATH)
%.o:$(BASESRCPATH)/%.c
	$(CC) $(CFLAGS) -c $< -I$(BASEINCLUDEPATH)
%.o:$(SYSTEMSRCPATH)/%.c
	$(CC) $(CFLAGS) -c $< -I$(SYSTEMINCLUDEPATH) -I$(BASEINCLUDEPATH)
%.o:$(SAFETYSRCPATH)/%.c
	$(CC) -c $< -I$(SAFETYINCLUDEPATH) -I$(BASEINCLUDEPATH)

$(PROJECT):$(OBJECTLIST)
	$(CC) -o $@ $^

list:
	@echo "IncludePath:$(INCLUDEPATH)"
	@echo "SrcList:$(SRCFILE)"
build:
	@echo "the project is built..."
	@sudo mkdir -p $(BINDIR)
	@sudo mv *.o $(BINDIR)
	@sudo mv $(PROJECT) $(BINDIR)
clean:
	sudo rm $(BINDIR)/*.o $(BINDIR)/*.d $(BINDIR)/$(PROJECT)
