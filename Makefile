working: working.o token.o
	gcc -g -pthread working.o token.o -o detector -lm
working.o: working.c
	gcc -g -c -pthread working.c -lm
token.o: token.c
	gcc -g -c token.c -lm
clean:
	rm -f detector *.o
