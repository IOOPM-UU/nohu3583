CC      = gcc
CFLAGS  = -Wall -Wextra -pedantic -std=c11 -g
LDFLAGS = -lcunit

# Test targets
HASH_TARGET      = unit_tests
LINKED_TARGET    = linked_tests
ITERATOR_TARGET  = iterator_test

# Object files
HASH_OBJS      = unit_tests.o hash_table.o linked_list.o
LINKED_OBJS    = linked_tests.o linked_list.o
ITERATOR_OBJS  = iterator_test.o iterator.o linked_list.o

# Default: build all
all: $(HASH_TARGET) $(LINKED_TARGET) $(ITERATOR_TARGET)

# Generic rule to build executables from object files
%_test: $(_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Explicit linking rules
$(HASH_TARGET): $(HASH_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(LINKED_TARGET): $(LINKED_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(ITERATOR_TARGET): $(ITERATOR_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compilation rules (generic pattern)
%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

# Special cases: if header missing in pattern rule
unit_tests.o: unit_tests.c hash_table.h linked_list.h
linked_tests.o: linked_tests.c linked_list.h
iterator_test.o: iterator_test.c iterator.h linked_list.h
iterator.o: iterator.c iterator.h linked_list.h
hash_table.o: hash_table.c hash_table.h
linked_list.o: linked_list.c linked_list.h

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

# Run each test individually
run_hash: $(HASH_TARGET)
	./$(HASH_TARGET)

run_linked: $(LINKED_TARGET)
	./$(LINKED_TARGET)

run_iterator: $(ITERATOR_TARGET)
	./$(ITERATOR_TARGET)

# Run all tests together
run_all: $(HASH_TARGET) $(LINKED_TARGET) $(ITERATOR_TARGET)
	./$(HASH_TARGET)
	./$(LINKED_TARGET)
	./$(ITERATOR_TARGET)

# Clean up
clean:
	rm -f *.o $(HASH_TARGET) $(LINKED_TARGET) $(ITERATOR_TARGET) a.out
