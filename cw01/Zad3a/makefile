all: static_tests shared_tests

static_tests:
	gcc -c lib.c
	ar rcs libstatic.a lib.o
	rm -rf *.o
	gcc main.c -o test_static -L. -lstatic -Wall

shared_tests:
	gcc -c -fpic lib.c
	gcc -shared -o libshared.so lib.o
	rm -rf *.o
	gcc -L. -Wl,-rpath=. -o test_shared main.c -lshared -Wall

results:
	echo "Wyniki dla biblioteki statycznej:" >> results3a.txt
	./test_static 5 stworz 10000 10000 wypelnij wyszukaj kolejno 1000 5000 na_przemian 1000 5000 >> results3a.txt
	echo "   " >> results3a.txt
	echo "Wyniki dla biblioteki dzielonej:" >> results3a.txt
	./test_shared 5 stworz 10000 10000 wypelnij wyszukaj kolejno 1000 5000 na_przemian 1000 5000 >> results3a.txt
	


