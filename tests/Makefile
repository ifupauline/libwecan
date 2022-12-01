SRC_DIR += src
OBJ_DIR += obj
BIN_DIR += bin

EXE += $(BIN_DIR)/tests
SRC += $(wildcard $(SRC_DIR)/*.c)

OBJ += $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CFLAGS += -Wall
CFLAGS += -MD
CFLAGS += -I../include/

LDLIBS += -L../lib/ -lwecan
LDLIBS += -Wl,-rpath=../../lib


.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CXX) -o $@ $^ $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR)

-include $(OBJ:.o=.d)
