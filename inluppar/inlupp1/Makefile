# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
LDFLAGS = -lcunit

# Source files
COMMON_SRC = common.c
LINKED_LIST_SRC = linked_list.c
HASH_TABLE_SRC = hash_table.c
ITERATOR_SRC = iterator.c

# Main programs
ITERATOR_TEST_SRC = iterator_test.c
LINKED_TESTS_SRC = linked_tests.c
UNIT_TESTS_SRC = unit_tests.c
FREQ_COUNT_SRC = freq-count.c

# Object files
COMMON_OBJ = common.o
LINKED_LIST_OBJ = linked_list.o
HASH_TABLE_OBJ = hash_table.o
ITERATOR_OBJ = iterator.o

# Executables
ITERATOR_TEST = iterator_test
LINKED_TESTS = linked_tests
UNIT_TESTS = unit_tests
FREQ_COUNT = freq-count

# Default target
all: $(FREQ_COUNT) $(ITERATOR_TEST) $(LINKED_TESTS) $(UNIT_TESTS)

# Object file rules
$(COMMON_OBJ): $(COMMON_SRC) common.h
	$(CC) $(CFLAGS) -c $(COMMON_SRC) -o $(COMMON_OBJ)

$(LINKED_LIST_OBJ): $(LINKED_LIST_SRC) linked_list.h common.h
	$(CC) $(CFLAGS) -c $(LINKED_LIST_SRC) -o $(LINKED_LIST_OBJ)

$(HASH_TABLE_OBJ): $(HASH_TABLE_SRC) hash_table.h common.h
	$(CC) $(CFLAGS) -c $(HASH_TABLE_SRC) -o $(HASH_TABLE_OBJ)

$(ITERATOR_OBJ): $(ITERATOR_SRC) iterator.h linked_list.h common.h
	$(CC) $(CFLAGS) -c $(ITERATOR_SRC) -o $(ITERATOR_OBJ)

# Executable rules
$(FREQ_COUNT): $(FREQ_COUNT_SRC) $(COMMON_OBJ) $(LINKED_LIST_OBJ) $(HASH_TABLE_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(ITERATOR_TEST): $(ITERATOR_TEST_SRC) $(COMMON_OBJ) $(LINKED_LIST_OBJ) $(ITERATOR_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(LINKED_TESTS): $(LINKED_TESTS_SRC) $(LINKED_LIST_OBJ) $(COMMON_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(UNIT_TESTS): $(UNIT_TESTS_SRC) $(COMMON_OBJ) $(LINKED_LIST_OBJ) $(HASH_TABLE_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Test targets with clean after
test_unit: $(UNIT_TESTS)
	./$(UNIT_TESTS)
	$(MAKE) clean

test_linked: $(LINKED_TESTS)
	./$(LINKED_TESTS)
	$(MAKE) clean

test_iterator: $(ITERATOR_TEST)
	./$(ITERATOR_TEST)
	$(MAKE) clean

test_all: $(UNIT_TESTS) $(LINKED_TESTS) $(ITERATOR_TEST)
	./$(UNIT_TESTS)
	./$(LINKED_TESTS)
	./$(ITERATOR_TEST)
	$(MAKE) clean

# Memory test targets with clean after
memtest_unit: $(UNIT_TESTS)
	valgrind --leak-check=full ./$(UNIT_TESTS)
	$(MAKE) clean

memtest_linked: $(LINKED_TESTS)
	valgrind --leak-check=full ./$(LINKED_TESTS)
	$(MAKE) clean

memtest_iterator: $(ITERATOR_TEST)
	valgrind --leak-check=full ./$(ITERATOR_TEST)
	$(MAKE) clean

memtest_all: $(UNIT_TESTS) $(LINKED_TESTS) $(ITERATOR_TEST)
	valgrind --leak-check=full ./$(UNIT_TESTS)
	valgrind --leak-check=full ./$(LINKED_TESTS)
	valgrind --leak-check=full ./$(ITERATOR_TEST)
	$(MAKE) clean

# Simple freq-count targets
run_freq: $(FREQ_COUNT)
	./$(FREQ_COUNT) $(FILE)

memrun_freq: $(FREQ_COUNT)
	valgrind --leak-check=full ./$(FREQ_COUNT) $(FILE)

# Individual build targets
build_freq: $(FREQ_COUNT)
build_iterator_test: $(ITERATOR_TEST)
build_linked_tests: $(LINKED_TESTS)
build_unit_tests: $(UNIT_TESTS)

# Clean target
clean:
	rm -f *.o $(FREQ_COUNT) $(ITERATOR_TEST) $(LINKED_TESTS) $(UNIT_TESTS)

# Phony targets
.PHONY: all clean test_all memtest_all test_unit test_linked test_iterator \
        memtest_unit memtest_linked memtest_iterator run_freq memrun_freq \
        build_freq build_iterator_test build_linked_tests build_unit_tests