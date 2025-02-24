P3-sem-gavinward:  P3-sem-gavinward.o mytime.o
	gcc -Wall -std=c99 -lpthread -o P3-sem-gavinward P3-sem-gavinward.o mytime.o
P3-sem-gavinward.o: P3-sem-gavinward.c mytime.h
	gcc -c P3-sem-gavinward.c -lpthread -Wall -std=c99
mytime.o: mytime.c
	gcc -c mytime.c -Wall -std=c99
clean:
	rm -f *.o P3-sem-gavinward