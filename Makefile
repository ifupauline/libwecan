SRC_DIR += src
OBJ_DIR += obj
LIB_DIR += lib

LIB += $(LIB_DIR)/libwecan.a
SRC += $(wildcard $(SRC_DIR)/*.c)
OBJ += $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CFLAGS += -Wall
CFLAGS += -Iinclude/

.PHONY: all clean

all: $(LIB)

$(LIB): $(OBJ) | $(LIB_DIR)
	ar rc $(LIB) $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	@$(RM) -rv $(LIB_DIR) $(OBJ_DIR)

-include $(OBJ:.o=.d)
