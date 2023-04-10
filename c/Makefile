CFLAGS = -g -pedantic 
include = ./include
src = ./src
out = ./out
lib = ./lib
bin = ./bin

all: api converter

api: $(bin)/apiRequest
$(bin)/apiRequest: $(src)/apiRequest.c
	mkdir -p $(bin)
	gcc $(CFLAGS) -o $@ $^ -lcurl -lcjson

converter: $(bin)/converter 
$(bin)/converter: $(src)/converter.c $(out)/mult.o $(out)/asm_io.o
	gcc $(CFLAGS) -o $@ -m32 $^

$(out)/mult.o: $(src)/mult.asm
	mkdir -p $(out)/
	nasm -f elf32 -o $@ $^

$(out)/asm_io.o: $(src)/asm_io.asm
	nasm -f elf32 -d ELF_TYPE -o $@ $^

clean:
	rm -rf $(bin)/ $(out)/
