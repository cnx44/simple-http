CC       = gcc
CFLAGS   = -Wall -Wextra -O2 -std=c11
CPPFLAGS = -Iinclude -MMD -MP

SRC  = $(wildcard src/*.c)
OBJ  = $(patsubst src/%.c, build/%.o, $(SRC))
DEP  = $(OBJ:.o=.d)

TARGET = http_server

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

build/%.o: src/%.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

build:
	mkdir -p build

clean:
	rm -rf build $(TARGET)

# Target per compilare in debug
debug: CFLAGS = -Wall -Wextra -g -O0 -std=c11
debug: clean $(TARGET)

-include $(DEP)

.PHONY: clean debug
