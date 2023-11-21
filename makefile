PROJECT = train
CC = gcc
CFLAGS += -DDEBUG_MODE
CFLAGS += -D INTEGERTYPE=0
CFLAGS += -D FLOATINGTYPE=1
CFLAGS += -D MATRIX_STORETYPE=INTEGERTYPE
MODULE0 := base
MODULE1 := system
BASEINCLUDEPATH := ./include/$(MODULE0)/
SYSTEMINCLUDEPATH := ./include/$(MODULE1)/
INCLUDEPATH = $(BASEINCLUDEPATH) $(SYSTEMINCLUDEPATH) 
MAINSRCPATH = .
BASESRCPATH = ./src/$(MODULE0)/
SYSTEMSRCPATH = ./src/$(MODULE1)/
SRCLIST += $(wildcard $(MAINSRCPATH)/*.c)
SRCLIST += $(wildcard $(BASESRCPATH)/*.c)
SRCLIST += $(wildcard $(SYSTEMSRCPATH)/*.c)
SRCFILE = $(notdir $(SRCLIST))
OBJECTLIST = $(SRCFILE:.c=.o)

BINDIR = ./bin
BINPATH = $(pwd)/bin/

%.o:$(MAINSRCPATH)/%.c
	$(CC) $(CFLAGS) -c $< -I$(SYSTEMINCLUDEPATH) -I$(BASEINCLUDEPATH) -MD -MF $(PROJECT).d
%.o:$(BASESRCPATH)/%.c
	$(CC) $(CFLAGS) -c $< -I$(BASEINCLUDEPATH) -MD -MF $(PROJECT).d
%.o:$(SYSTEMSRCPATH)/%.c
	$(CC) $(CFLAGS) -c $< -I$(SYSTEMINCLUDEPATH) -I$(BASEINCLUDEPATH) -MD -MF $(PROJECT).d

$(PROJECT):$(OBJECTLIST)
	$(CC) -o $@ $^

list:
	@echo "IncludePath:$(INCLUDEPATH)"
	@echo "SrcList:$(SRCFILE)"
build:
	@echo "the project is built..."
	@sudo mkdir -p $(BINDIR)
	@sudo mv *.o $(BINDIR)
	@sudo mv *.d $(BINDIR)
	@sudo mv $(PROJECT) $(BINDIR)
clean:
	sudo rm $(BINDIR)/*.o $(BINDIR)/*.d $(BINDIR)/$(PROJECT)
