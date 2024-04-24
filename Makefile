CC:= gcc

PROG:= prog

INCLUDE:= -I./include/

FLAGS:= -g -std=c11 -Wall -Wextra -Wpedantic 

C_SRC:= $(wildcard ./src/*.c ./src/**/*.c)

TARGET:= ./bin/$(PROG)

all:
	$(CC) $(FLAGS)  $(INCLUDE) $(C_SRC) -o $(TARGET) && $(TARGET)


val:
	valgrind -s --leak-check=full --track-origins=yes $(TARGET) 

gdb:
	gdb $(TARGET)

test:
	$(TARGET) ./input/test.txt
