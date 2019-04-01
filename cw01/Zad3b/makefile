all: static_tests shared_tests

static_tests:
	gcc -c lib.c
	ar rcs libstatic.a lib.o
	rm -rf *.o
	gcc main.c -o test_static_O -L. -lstatic -Wall -O
	gcc main.c -o test_static_O1 -L. -lstatic -Wall -O1
	gcc main.c -o test_static_O2 -L. -lstatic -Wall -O2
	gcc main.c -o test_static_O3 -L. -lstatic -Wall -O3
	gcc main.c -o test_static_Os -L. -lstatic -Wall -Os

shared_tests:
	gcc -c -fpic lib.c
	gcc -shared -o libshared.so lib.o
	rm -rf *.o
	gcc -L. -Wl,-rpath=. -o test_shared_O main.c -lshared -Wall -O
	gcc -L. -Wl,-rpath=. -o test_shared_O1 main.c -lshared -Wall -O1
	gcc -L. -Wl,-rpath=. -o test_shared_O2 main.c -lshared -Wall -O2
	gcc -L. -Wl,-rpath=. -o test_shared_O3 main.c -lshared -Wall -O3
	gcc -L. -Wl,-rpath=. -o test_shared_Os main.c -lshared -Wall -Os

results:
	echo "Wyniki dla biblioteki statycznej -O:" >> results3b.txt
	./test_static_O 5 stworz 10000 10000 wypelnij wyszukaj kolejno 1000 5000 na_przemian 1000 5000 >> results3b.txt
	echo "   " >> results3b.txt
	echo "Wyniki dla biblioteki statycznej -O1:" >> results3b.txt
	./test_static_O1 5 stworz 10000 10000 wypelnij wyszukaj kolejno 1000 5000 na_przemian 1000 5000 >> results3b.txt
	echo "   " >> results3b.txt
	echo "Wyniki dla biblioteki statycznej -O2:" >> results3b.txt
	./test_static_O2 5 stworz 10000 10000 wypelnij wyszukaj kolejno 1000 5000 na_przemian 1000 5000 >> results3b.txt
	echo "   " >> results3b.txt
	echo "Wyniki dla biblioteki statycznej -O3:" >> results3b.txt
	./test_static_O3 5 stworz 10000 10000 wypelnij wyszukaj kolejno 1000 5000 na_przemian 1000 5000 >> results3b.txt
	echo "   " >> results3b.txt
	echo "Wyniki dla biblioteki statycznej -Os:" >> results3b.txt
	./test_static_Os 5 stworz 10000 10000 wypelnij wyszukaj kolejno 1000 5000 na_przemian 1000 5000 >> results3b.txt
	echo "   " >> results3b.txt
	echo "Wyniki dla biblioteki dzielonej -O:" >> results3b.txt
	./test_shared_O 5 stworz 10000 10000 wypelnij wyszukaj kolejno 1000 5000 na_przemian 1000 5000 >> results3b.txt
	echo "   " >> results3b.txt
	echo "Wyniki dla biblioteki dzielonej -O1:" >> results3b.txt
	./test_shared_O1 5 stworz 10000 10000 wypelnij wyszukaj kolejno 1000 5000 na_przemian 1000 5000 >> results3b.txt
	echo "   " >> results3b.txt
	echo "Wyniki dla biblioteki dzielonej -O2:" >> results3b.txt
	./test_shared_O2 5 stworz 10000 10000 wypelnij wyszukaj kolejno 1000 5000 na_przemian 1000 5000 >> results3b.txt
	echo "   " >> results3b.txt
	echo "Wyniki dla biblioteki dzielonej -O3:" >> results3b.txt
	./test_shared_O3 5 stworz 10000 10000 wypelnij wyszukaj kolejno 1000 5000 na_przemian 1000 5000 >> results3b.txt
	echo "   " >> results3b.txt
	echo "Wyniki dla biblioteki dzielonej -Os:" >> results3b.txt
	./test_shared_Os 5 stworz 10000 10000 wypelnij wyszukaj kolejno 1000 5000 na_przemian 1000 5000 >> results3b.txt
	


