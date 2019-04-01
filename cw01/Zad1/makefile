all: static shared

static:
	gcc -c lib.c
	ar rcs libstatic.a lib.o
	rm -rf *.o
shared:
	gcc -fPIC -c lib.c
	gcc -shared -o libshared.so lib.o
	rm -rf *.o
