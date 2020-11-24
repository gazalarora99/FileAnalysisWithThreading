all: Asst2

Asst2: Asst2.c
	gcc -fsanitize=address -o detector Asst2.c -lpthread
clean:
	rm -f detector
