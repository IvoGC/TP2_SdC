CFLAGS = -Wall -Werror -pedantic 
include = ./include
src = ./src
out = ./out
lib = ./lib
bin = ./bin

$(lib)/librequests.so: $(out)/requests.o
	mkdir -p $(lib)/
	gcc $(CFLAGS) -shared -o $@ $^

$(out)/requests.o: $(src)/requests.c
	mkdir -p $(out)/
	gcc $(CFLAGS) -o $@ -c $^


clean:
	rm -rf $(out)/ $(lib)/ 