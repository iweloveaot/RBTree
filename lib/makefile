
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic
DEBUGFLAGS = -g -O0 -DDEBUG
RELEASEFLAGS = -O2 -DNDEBUG

APP_TARGET = rb_tree_app
TEST_TARGET = rb_tree_tests

APP_SRC = main.cpp
TEST_SRC = rb_tests_main.cpp

HEADERS = rbTree.h \
           dataTypes.h \
           rb_tree_tests.h \
		   types/person.h \
           types/date.h \
           types/student.h \
           types/teacher.h \
           types/functionValue.h

.PHONY: all
all: build_app build_tests

.PHONY: build_app
build_app: $(APP_TARGET)

.PHONY: build_tests
build_tests: $(TEST_TARGET)

$(APP_TARGET): $(APP_SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(RELEASEFLAGS) -o $@ $(APP_SRC)

$(TEST_TARGET): $(TEST_SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) -o $@ $(TEST_SRC)

# Debug-сборка тестов (для прогона под valgrind / gdb)
.PHONY: build_tests_debug
build_tests_debug:
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) -fsanitize=address,undefined -o $(TEST_TARGET)_asan $(TEST_SRC)


.PHONY: run_app
run_app: $(APP_TARGET)
	@echo ">>> Запуск основной программы..."
	@./$(APP_TARGET)

.PHONY: run_tests
run_tests: $(TEST_TARGET)
	@echo ">>> Запуск тестов..."
	@./$(TEST_TARGET)

.PHONY: run_tests_debug
run_tests_debug: build_tests_debug
	@echo ">>> Запуск тестов под ASan/UBSan..."
	@./$(TEST_TARGET)_asan


.PHONY: clean
clean:
	rm -f $(APP_TARGET) $(TEST_TARGET) $(TEST_TARGET)_asan

.PHONY: help
help:
	@echo "Доступные цели:"
	@echo "  make all                — собрать и программу, и тесты (по умолчанию)"
	@echo "  make build_app          — собрать только основную программу"
	@echo "  make build_tests        — собрать только тесты (с отладочной информацией)"
	@echo "  make build_tests_debug  — собрать тесты с ASan + UBSan"
	@echo "  make run_app            — собрать и запустить программу"
	@echo "  make run_tests          — собрать и запустить тесты"
	@echo "  make run_tests_debug    — собрать и прогнать тесты под санитизерами"
	@echo "  make clean              — удалить все бинарники"
	@echo "  make help               — показать эту справку"

.PHONY: all build_app build_tests build_tests_debug \
        run_app run_tests run_tests_debug \
        clean help