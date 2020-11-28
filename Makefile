working: working.o token.o
	gcc -g -pthread working.o token.o -o detector
working.o: working.c
	gcc -g -c -pthread working.c
token.o: token.c
	gcc -g -c token.c
clean:
	rm -f detector *.o
