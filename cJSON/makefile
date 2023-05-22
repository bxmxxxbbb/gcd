LDFLAGS = -lm
OBJES = cJSON.o test.o

MAIN: $(OBJES) 
		gcc $(OBJES) -o MAIN $(LDFLAGS)
test.o:test.c cJSON.h
		gcc -c test.c -o test.o
cJSON.o:cJSON.c cJSON.h
		gcc -c cJSON.c -o cJSON.o
clean:
		rm -rf *.o MAIN

