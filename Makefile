pc2: pc2.o tokenizer.o
	gcc -pthread pc2.o tokenizer.o -o detector
pc2.o: pc2.c
	gcc -c -pthread pc2.c
tokenizer.o: tokenizer.c
	gcc -c tokenizer.c
clean:
	rm -f detector *.o
