CC      = gcc
CFLAGS  = -Wall -Wextra -pedantic -std=c11 -g
LDFLAGS = -lcunit

# Targets
HASH_TARGET    = hash_tests
HASH_OBJS      = unit_tests.o hash_table.o

LINKED_TARGET  = linked_tests
LINKED_OBJS    = linked_tests.o linked_list.o

# Default: build everything
all: $(HASH_TARGET) $(LINKED_TARGET)

# Hash table tests
$(HASH_TARGET): $(HASH_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	./$@

unit_tests.o: unit_tests.c hash_table.h
	$(CC) $(CFLAGS) -c unit_tests.c -o unit_tests.o

hash_table.o: hash_table.c hash_table.h
	$(CC) $(CFLAGS) -c hash_table.c -o hash_table.o

# Linked list tests
$(LINKED_TARGET): $(LINKED_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	./$@

linked_tests.o: linked_tests.c linked_list.h
	$(CC) $(CFLAGS) -c linked_tests.c -o linked_tests.o

linked_list.o: linked_list.c linked_list.h
	$(CC) $(CFLAGS) -c linked_list.c -o linked_list.o

# Run both test suites
all_tests: $(HASH_TARGET) $(LINKED_TARGET)
	./$(HASH_TARGET)
	./$(LINKED_TARGET)

# Memory check
memtest_hash: $(HASH_TARGET)
	valgrind --leak-check=full ./$(HASH_TARGET)

memtest_linked: $(LINKED_TARGET)
	valgrind --leak-check=full ./$(LINKED_TARGET)

# Cleanup
clean:
	rm -f $(HASH_OBJS) $(LINKED_OBJS) $(HASH_TARGET) $(LINKED_TARGET) a.out
