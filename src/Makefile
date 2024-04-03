PROJECT_NAME= c7_tetris
CC= gcc
GCOV= --coverage
LIBS= -lncurses -lm
FRONTEND= gui/cli
BACKEND= brick_game/tetris
TEST_DIR= tetris_tests
CFLAGS= -Wall -Werror -Wextra -std=c11
CLANG= clang-format

OS=$(shell uname -s)

ifeq ($(OS), Linux)
	LEAK_CHECK=valgrind --tool=memcheck --leak-check=yes
	OPEN_CMD=xdg-open
	ADD_LIB=-lpthread -lcheck_pic -lrt -lsubunit
else ifeq ($(OS), Darwin)
	LEAK_CHECK=leaks -atExit --
	OPEN_CMD=open
	ADD_LIB=-lm -lcheck -lpthread
endif


all: install uninstall dvi dist test gcov_report clean

install: clean
	mkdir build
	cd build && $(CC) $(CFLAGS) ../$(BACKEND)/*.c ../$(FRONTEND)/*.c $(LIBS) -o $(PROJECT_NAME) 
	./build/$(PROJECT_NAME)

uninstall:
	rm -rf build


dvi: clean
	doxygen
	open ./doxygen/html/index.html
	open ./README.md
	open ./FSM_tetris.jpeg

dist: clean
	mkdir $(PROJECT_NAME)
	cp -r brick_game $(PROJECT_NAME)
	cp -r gui $(PROJECT_NAME)
	cp Makefile $(PROJECT_NAME)
	cp Doxyfile $(PROJECT_NAME)
	tar -cvzf $(PROJECT_NAME).tgz $(PROJECT_NAME)
	rm -rf $(PROJECT_NAME)

test: clean
	$(CC) $(CFLAGS) $(BACKEND)/*.c $(TEST_DIR)/*.c $(LIBS) -o tests $(ADD_LIB) $(LIBS)
	./tests

gcov_report: clean
	$(CC) $(CFLAGS) $(BACKEND)/*.c $(TEST_DIR)/*.c $(LIBS) -o gcov_tests $(ADD_LIB) $(GCOV)
	./gcov_tests
	lcov -c -d . -o gcov_tests.info
	genhtml -o report gcov_tests.info
	chmod 777 report/*
	rm *.gcda *.gcno
	$(OPEN_CMD) ./report/index.html


clean: uninstall
	rm -rf *.o *.a test *.gch *.gcno *.gcda *.info report doxygen 
	rm -rf $(BACKEND)/test_file.txt $(PROJECT_NAME)
	rm -rf $(PROJECT_NAME).tgz gcov_tests test_file.txt tests

clang_n:
	cp ../materials/linters/.clang-format .
	$(CLANG) -style=Google -n $(BACKEND)/*.[ch] $(FRONTEND)/*.[ch] $(TEST_DIR)/*.[ch]
	rm -rf .clang-format

clang_i:
	cp ../materials/linters/.clang-format .
	$(CLANG) -style=Google -i $(BACKEND)/*.[ch] $(FRONTEND)/*.[ch] $(TEST_DIR)/*.[ch]
	rm -rf .clang-format

leaks: clean test
	$(LEAK_CHECK) ./tests