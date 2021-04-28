SRC_DIR += .
OBJ_DIR += obj
LIB_DIR += ../lib

EXE += tests
SRC += $(wildcard tests.c)
OBJ += $(SRC:%.c=$(OBJ_DIR)/%.o)
LIB += libwecan

CFLAGS += -I../include/
LDLIBS += -L../lib/ -lwecan

.PHONY: all clean

all: $(EXE) 

$(EXE): $(OBJ) | $(LIB)
	$(CC) -o $(EXE) $< $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

$(LIB):
	cd ../; make

clean:
	@$(RM) -rv $(OBJ_DIR) $(EXE)

-include $(OBJ:.o=.d)
