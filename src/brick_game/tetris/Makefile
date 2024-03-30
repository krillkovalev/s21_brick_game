CC = gcc
CFLAGS = -I inc -std=c11 -Wall -Werror -Wpedantic -lncurses
LOGIC = s21_other_functions.c fsm_tetris.c
GAME = s21_tetris.c
RENDERING = ../../gui/cli/s21_tetris_rendering.c
TEST = tests/s21_*.c
OBJECTS = *.o



OS = $(shell uname)

ifeq ($(OS), Darwin) 
 TEST_LIBS=-lcheck
else
 TEST_LIBS=-lcheck -lsubunit -pthread -lrt
endif

s21_brick_game.a:
	$(CC) $(CFLAGS) -c $(LOGIC) $(RENDERING)
	ar rcs s21_brick_game.a $(OBJECTS)

install:
	$(CC) $(CFLAGS) -o s21_tetris $(LOGIC) $(RENDERING) $(GAME)
	./s21_tetris

uninstall:
	rm -rf s21_tetris

dvi:
	doxygen
	open ./doxygen/html/index.html

dist:

test: s21_brick_game.a
	$(CC) $(CFLAGS) $(TEST) s21_brick_game.a -o test $(TEST_LIBS)
	./test
	make clean

gcov_report: s21_brick_game.a
	$(CC) $(CFLAGS) --coverage $(LOGIC) $(TEST_LIBS) -o test
	./test
	lcov -t "test" -o test_report.info -c -d . #--rc lcov_branch_coverage=1
	genhtml -o report test_report.info #--rc lcov_branch_coverage=1
	rm -rf *.o *.a *.gch *.gcno test
	open ./report/index.html 


clean:
	rm -rf *.o *.a test *.gch *.gcno *.gcda *.info report s21_tetris test_file.txt 
 

