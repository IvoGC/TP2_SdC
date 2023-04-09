CFLAGS = -Wall -pedantic 
include = ./include
src = ./src
out = ./out
lib = ./lib
bin = ./bin

$(lib)/librequests.so: $(out)/requests.o $(out)/mult.o
	mkdir -p $(lib)/
	gcc $(CFLAGS) -shared -o $@ $^

$(out)/requests.o: $(src)/requests.c
	mkdir -p $(out)/
	gcc $(CFLAGS) -o $@ -c -fPIC $^ 

$(out)/mult.o: $(src)/mult.asm
	nasm -f elf64 -o $@ $^

clean:
	rm -rf $(out)/ $(lib)/ 