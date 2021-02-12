CC = gcc
CFLAGS= -Wall 
LDFLAGS = -lm
LIBS = 
INCLUDE = -I./include
SRC_DIR = ./src
OBJ_DIR = ./build
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(subst $(SRC_DIR),$(OBJ_DIR), $(SOURCES:.c=.o))
TARGET = ./bin/advance


$(TARGET): $(OBJECTS) $(LIBS) 
	$(CC) -o $@ $^ $(LDFLAGS)


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@if [ ! -d $(OBJ_DIR) ]; \
      then echo "mkdir -p $(OBJ_DIR)"; mkdir -p $(OBJ_DIR); \
      fi
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean: 
	rm -f $(OBJECTS) $(TARGET)
