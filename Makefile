SRC_DIR += src
OBJ_DIR += obj
LIB_DIR += lib
INC_DIR += include
LIB_NAME += libwecan
LIB_INSTALL_PATH += $(LIB_DIR)/$(LIB_NAME)

HDR += $(INC_DIR)/libwecan.h
LIB += $(LIB_DIR)/libwecan.a
SRC += $(wildcard $(SRC_DIR)/*.c)
OBJ += $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CFLAGS += -Wall
CFLAGS += -Iinclude/

ifeq ($(PREFIX),)
		PREFIX := /usr/local
endif

.PHONY: all clean

all: $(LIB)

$(LIB): $(OBJ) | $(LIB_DIR)
	ar rc $(LIB) $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB_DIR) $(OBJ_DIR):
	mkdir -p $@

install:
	install -d $(PREFIX)/$(LIB_INSTALL_PATH)/
	install -m 644 $(LIB) $(PREFIX)/$(LIB_INSTALL_PATH)/
	install -d $(PREFIX)/$(INC_DIR)/
	install -m 644 $(HDR) $(PREFIX)/$(INC_DIR)/

uninstall:
	rm -rf $(PREFIX)/$(LIB_DIR)/$(LIB_NAME)
	rm -rf $(PREFIX)/$(HDR)

clean:
	@$(RM) -rv $(LIB_DIR) $(OBJ_DIR)

-include $(OBJ:.o=.d)
