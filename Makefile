$(VERBOSE).SILENT:
all: test1

test1: 
	@gcc -Wall SERVER.c -o ttts -lpthread
	@gcc  -Wall CLIENT.c -o ttt

clean:
	@rm -f ttts
	@rm -f ttt