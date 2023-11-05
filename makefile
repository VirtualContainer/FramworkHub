PROJECT = train
CC = gcc
CFLAGS += -DDEBUG_MODE
MODULE0 := base
SRCPATH = ./src/$(MODULE0)/
INCLUDEPATH = ./include/$(MODULE0)/
SRCLIST = $(wildcard $(SRCPATH)/*.c)
OBJECTLIST = $(patsubst $(SRCPATH)/%.c,%.o,$(SRCLIST))
BINDIR = ./bin
BINPATH = $(pwd)/bin/
%.o:$(SRCPATH)/%.c
	$(CC) $(CFLAGS) -c $< -I$(INCLUDEPATH) -MD -MF $(PROJECT).d
$(PROJECT):$(OBJECTLIST)
	$(CC) -o $@ $^	
build:
	@echo "the project is built..."
	@sudo mkdir -p $(BINDIR)
	@sudo mv *.o $(BINDIR)
	@sudo mv *.d $(BINDIR)
	@sudo mv $(PROJECT) $(BINDIR)
clean:
	sudo rm $(BINDIR)/*.o $(BINDIR)/*.d $(BINDIR)/$(PROJECT)
