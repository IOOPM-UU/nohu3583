CC      = gcc
CFLAGS  = -Wall -Wextra -pedantic -std=c11 -g
LDFLAGS = -lcunit

# Targets
HASH_TARGET    = hash_tests
HASH_OBJS      = unit_tests.o hash_table.o

LINKED_TARGET  = linked_tests
LINKED_OBJS    = linked_tests.o linked_list.o

ITERATOR_TARGET = iterator_test
ITERATOR_OBJS   = iterator_test.o iterator.o linked_list.o

# Default: build everything (just compile)
all: $(HASH_TARGET) $(LINKED_TARGET) $(ITERATOR_TARGET)

# Hash table tests
$(HASH_TARGET): $(HASH_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

unit_tests.o: unit_tests.c hash_table.h
	$(CC) $(CFLAGS) -c $< -o $@

hash_table.o: hash_table.c hash_table.h
	$(CC) $(CFLAGS) -c $< -o $@

# Linked list tests
$(LINKED_TARGET): $(LINKED_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

linked_tests.o: linked_tests.c linked_list.h
	$(CC) $(CFLAGS) -c $< -o $@

linked_list.o: linked_list.c linked_list.h
	$(CC) $(CFLAGS) -c $< -o $@

# Iterator tests
$(ITERATOR_TARGET): $(ITERATOR_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

iterator_test.o: iterator_test.c iterator.h linked_list.h
	$(CC) $(CFLAGS) -c $< -o $@

iterator.o: iterator.c iterator.h linked_list.h
	$(CC) $(CFLAGS) -c $< -o $@

# Run all test suites
all_tests: $(HASH_TARGET) $(LINKED_TARGET) $(ITERATOR_TARGET)
	./$(HASH_TARGET)
	./$(LINKED_TARGET)
	./$(ITERATOR_TARGET)

# Memory checks
memtest_hash: $(HASH_TARGET)
	valgrind --leak-check=full ./$(HASH_TARGET)

memtest_linked: $(LINKED_TARGET)
	valgrind --leak-check=full ./$(LINKED_TARGET)

memtest_iterator: $(ITERATOR_TARGET)
	valgrind --leak-check=full ./$(ITERATOR_TARGET)

# Cleanup
clean:
	rm -f $(HASH_OBJS) $(LINKED_OBJS) $(ITERATOR_OBJS) \
	      $(HASH_TARGET) $(LINKED_TARGET) $(ITERATOR_TARGET) a.out
