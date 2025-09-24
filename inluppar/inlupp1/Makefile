CC      = gcc
CFLAGS  = -Wall -Wextra -pedantic -std=c11 -g
LDFLAGS = -lcunit

TARGET  = unit_tests
OBJS    = unit_tests.o hash_table.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

unit_tests.o: unit_tests.c hash_table.h
	$(CC) $(CFLAGS) -c unit_tests.c -o unit_tests.o

test: $(TARGET)
	./$(TARGET)

memtest: $(TARGET)
	valgrind --leak-check=full ./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) a.out

