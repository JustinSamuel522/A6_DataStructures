a6: A6.c
	gcc -o a6 A6.c

run: A6
	./a6 points.txt

clean: # remove all machine-generated files
	rm -f a6 *.o memcheck.txt *~
