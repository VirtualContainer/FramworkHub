PROJECT = train
CC = gcc
BASECFLAGS += -DDEBUG_MODE
MODULE0 := base
MODULE1 := system
BASEINCLUDEPATH := ./include/$(MODULE0)/
SYSTEMINCLUDEPATH := ./include/$(MODULE1)/
INCLUDEPATH = $(BASEINCLUDEPATH) $(SYSTEMINCLUDEPATH) 
BASESRCPATH = ./src/$(MODULE0)/
SYSTEMSRCPATH = ./src/$(MODULE1)/
SRCLIST += $(wildcard $(BASESRCPATH)/*.c)
SRCLIST += $(wildcard $(SYSTEMSRCPATH)/*.c)
SRCFILE = $(notdir $(SRCLIST))
OBJECTLIST = $(SRCFILE:.c=.o)

BINDIR = ./bin
BINPATH = $(pwd)/bin/

%.o:$(BASESRCPATH)/%.c
	$(CC) $(BASECFLAGS) -c $< -I$(BASEINCLUDEPATH) -MD -MF $(PROJECT).d
%.o:$(SYSTEMSRCPATH)/%.c
	$(CC) -c $< -I$(SYSTEMINCLUDEPATH) -I$(BASEINCLUDEPATH) -MD -MF $(PROJECT).d

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
