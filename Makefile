working: working.o token.o
	gcc -pthread working.o token.o -o detector
working.o: working.c
	gcc -c -pthread working.c
token.o: token.c
	gcc -c token.c
clean:
	rm -f detector *.o
