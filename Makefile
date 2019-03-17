#variables
CC=gcc
DEBUG=-DVERBOSE_DEBUG_
BIN=bin
OUT=tests/tests
SRC=tests/tests.c

.PHONY: all
all: 
	@if [ ! -d "$(BIN)" ]; then mkdir $(BIN); fi;
	$(CC) $(SRC) -o $(OUT);
	@mv $(OUT) $(BIN);

.PHONY: debug
debug:
	@if [ ! -d "$(BIN)" ]; then mkdir $(BIN); fi;
	$(CC) $(DEBUG) $(SRC) -o $(OUT);
	@mv $(OUT) $(BIN);

.PHONY: clean
clean:
	@if [ -d "$(BIN)" ]; then rm -r $(BIN); echo "rm -r $(BIN)"; fi;
