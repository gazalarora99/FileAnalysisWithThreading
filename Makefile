all: Asst2
Asst2: Asst2.o token.o
	gcc -g -pthread Asst2.o token.o -o detector -lm
Asst2.o: Asst2.c
	gcc -g -c -pthread Asst2.c -lm
token.o: token.c
	gcc -g -c token.c -lm
clean:
	rm -f detector *.o
