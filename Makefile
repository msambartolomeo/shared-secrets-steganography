TARGET = shared_secret_steganography
TARGET_DIR = target
OBJ_DIR = $(TARGET_DIR)/obj
INCLUDE_DIRS = ./src/include

SRCS  = $(wildcard src/*.c)
HEADERS = $(wildcard *.h) $(wildcard src/include/*.h)
INCLUDES = $(addprefix -I ,$(INCLUDE_DIRS))
OBJS = $(SRCS:%.c=$(OBJ_DIR)/%.o)

CFLAGS = -g -fsanitize=address -std=c2x -Wall -pedantic -pedantic-errors -Wextra -Werror -Wno-unused-parameter -Wno-implicit-fallthrough -lm -D_DEFAULT_SOURCE $(INCLUDES)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o target/$(TARGET) $(OBJS)

$(OBJ_DIR)/%.o: %.c $(HEADERS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ -c $<

run: all
	./$(TARGET_DIR)/$(TARGET)
	
clean:
	@rm -Rf $(OBJ_DIR) $(TARGET) $(TARGET_DIR)

format:
	@find . -regex '.*\.\(c\|h\)' -exec clang-format -i {} \;

.PHONY: all clean format