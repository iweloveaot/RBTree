CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra -Wpedantic -O0 -g

BUILD_DIR = build/wsl
TARGET = $(BUILD_DIR)/main
TEST_TARGET = $(BUILD_DIR)/tree_test
LOAD_TARGET = $(BUILD_DIR)/load_test
LOAD_FLAGS = -std=c++14 -Wall -Wextra -Wpedantic -O2
LOAD_ARGS = 1000000 3 all

SRCS = main.cpp
TEST_SRCS = tests/test_main.cpp tests/test_helpers.cpp tests/rbTree_test.cpp \
            tests/tree_algorithms_test.cpp tests/tree_string_test.cpp \
            tests/type_matrix_test.cpp

OBJS = $(addprefix $(BUILD_DIR)/,$(SRCS:.cpp=.o))
TEST_OBJS = $(addprefix $(BUILD_DIR)/,$(TEST_SRCS:.cpp=.o))
HEADERS = rbTree.h dataTypes.h $(wildcard types/*.h)
TEST_HEADERS = tests/test_helpers.h tests/type_fixtures.h

VALGRIND_FLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes \
                 --errors-for-leak-kinds=all --error-exitcode=1

.PHONY: all run test test-build valgrind valgrind-test load-test clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_TARGET): $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) -UNDEBUG -o $@ $^

$(BUILD_DIR)/main.o: main.cpp $(HEADERS)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/tests/%.o: tests/%.cpp $(HEADERS) $(TEST_HEADERS)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -UNDEBUG -c $< -o $@

run: $(TARGET)
	./$(TARGET)

test: $(TEST_TARGET) test-build
	./$(TEST_TARGET)

test-build:
	@set -e; \
	for case_name in CHECK_ASSERT MINIMAL_VALUE; do \
		echo "Checking $$case_name"; \
		$(CXX) $(CXXFLAGS) -DNDEBUG -UNDEBUG -D$$case_name \
			-fsyntax-only tests/compile_cases.cpp; \
	done
	@set -e; \
	for case_name in MERGE_KEY MERGE_VALUE MERGE_COMPARE \
	                 MISSING_OUTPUT MISSING_INPUT MISSING_EQUALITY; do \
		echo "Checking expected compilation failure: $$case_name"; \
		if $(CXX) $(CXXFLAGS) -UNDEBUG -D$$case_name \
			-fsyntax-only tests/compile_cases.cpp >/dev/null 2>&1; then \
			echo "ERROR: $$case_name unexpectedly compiled"; \
			exit 1; \
		fi; \
	done
	@set -e; \
	for header in $(HEADERS); do \
		echo "Checking header: $$header"; \
		printf '#include "%s"\nint main() {}\n' "$$header" | \
			$(CXX) $(CXXFLAGS) -UNDEBUG -I. -fsyntax-only -x c++ -; \
	done

valgrind: $(TARGET)
	valgrind $(VALGRIND_FLAGS) ./$(TARGET)

valgrind-test: $(TEST_TARGET)
	# Preserve the test operator new used to simulate allocation failures.
	valgrind $(VALGRIND_FLAGS) --soname-synonyms=somalloc=nouserintercepts ./$(TEST_TARGET)

$(LOAD_TARGET): load_tests.cpp $(HEADERS) makefile
	mkdir -p $(dir $@)
	$(CXX) $(LOAD_FLAGS) load_tests.cpp -o $@

load-test: $(LOAD_TARGET)
	./$(LOAD_TARGET) $(LOAD_ARGS)

clean:
	rm -f $(OBJS) $(TEST_OBJS) $(TARGET) $(TEST_TARGET) $(LOAD_TARGET)
