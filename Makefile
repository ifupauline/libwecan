SRC_DIR += src
OBJ_DIR += obj
LIB_DIR += lib

LIB += $(LIB_DIR)/libwecan.so
SRC += $(wildcard $(SRC_DIR)/*.c)
OBJ += $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CFLAGS += -g
CFLAGS += -Wall
CFLAGS += -fPIC
CFLAGS += -Iinclude/
CFLAGS += -MD

LDFLAGS += -shared

.PHONY: all clean

all: $(LIB) build-tests

$(LIB): $(OBJ) | $(LIB_DIR)
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS)  -o $@ -c $<

$(LIB_DIR) $(OBJ_DIR):
	mkdir -p $@

build-tests:
	cd tests/; make

run-tests:
	cd tests/bin/; ./tests

clean:
	@$(RM) -rv $(LIB_DIR) $(OBJ_DIR)
	cd tests/; make clean

-include $(OBJ:.o=.d)
