VAL = valgrind --leak-check=full --show-leak-kinds=all

CC = gcc
CFLAGS = -O3 -std=c99 -Wall -Wshadow -Wvla -pedantic

a6: A6.c
	$(CC) $(CFLAGS) -o a6 A6.c

run: a6
	./a6 in_file out_file1 out_file2 out_file3

testmemory: a6
	$(VAL) ./a6 in_file out_file1 out_file2 out_file3

clean:
	rm -f a6 out_file1.txt out_file2.txt out_file3.txt *.o memcheck.txt *~


