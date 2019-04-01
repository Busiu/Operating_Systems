all: compile

compile: main.c
	gcc $^ -o file_program -Wall

results: 
	echo "Wyniki dla kopiowania:" >> wyniki.txt
	echo "50000 rekordow, 4 bajty" >> wyniki.txt
	./file_program generate plik1 50000 4
	./file_program copy plik1 plik2 50000 4 sys >> wyniki.txt
	./file_program copy plik1 plik3 50000 4 lib >> wyniki.txt
	echo "100000 rekordow, 4 bajty" >> wyniki.txt
	./file_program generate plik1 100000 4
	./file_program copy plik1 plik2 100000 4 sys >> wyniki.txt
	./file_program copy plik1 plik3 100000 4 lib >> wyniki.txt
	echo "5000 rekordow, 512 bajty" >> wyniki.txt
	./file_program generate plik1 5000 512
	./file_program copy plik1 plik2 5000 512 sys >> wyniki.txt
	./file_program copy plik1 plik3 5000 512 lib >> wyniki.txt
	echo "10000 rekordow, 512 bajty" >> wyniki.txt
	./file_program generate plik1 10000 512
	./file_program copy plik1 plik2 10000 512 sys >> wyniki.txt
	./file_program copy plik1 plik3 10000 512 lib >> wyniki.txt
	echo "5000 rekordow, 4096 bajty" >> wyniki.txt
	./file_program generate plik1 5000 4096
	./file_program copy plik1 plik2 5000 4096 sys >> wyniki.txt
	./file_program copy plik1 plik3 5000 4096 lib >> wyniki.txt
	echo "10000 rekordow, 4096 bajty" >> wyniki.txt
	./file_program generate plik1 10000 4096
	./file_program copy plik1 plik2 10000 4096 sys >> wyniki.txt
	./file_program copy plik1 plik3 10000 4096 lib >> wyniki.txt
	echo "5000 rekordow, 8192 bajty" >> wyniki.txt
	./file_program generate plik1 5000 8192
	./file_program copy plik1 plik2 5000 8192 sys >> wyniki.txt
	./file_program copy plik1 plik3 5000 8192 lib >> wyniki.txt
	echo "10000 rekordow, 8192 bajty" >> wyniki.txt
	./file_program generate plik1 10000 8192
	./file_program copy plik1 plik2 10000 8192 sys >> wyniki.txt
	./file_program copy plik1 plik3 10000 8192 lib >> wyniki.txt
