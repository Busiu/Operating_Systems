# Zestaw 8
### Filtrowanie obrazów
Jedną z najprostszych operacji jaką można wykonać na obrazie jest operacja filtrowania (splotu). Operacja ta przyjmuje na wejściu dwie macierze:
* Macierz ![equation](https://latex.codecogs.com/gif.latex?I_{N\times%20M}) reprezentującą obraz. Dla uproszczenia rozważamy jedynie obrazy w 256 odcieniach szarości. Każdy element macierzy I jest więc liczbą całkowitą z zakresu 0 do 255.
* Macierz ![equation](https://latex.codecogs.com/gif.latex?K_{C\times%20C}) reprezentującą filtr. Elementami tej macierzy są liczby zmiennoprzecinkowe. Dla uproszczenia zakładamy, że elementy macierzy K sumują się do jedności:
![equation](https://latex.codecogs.com/gif.latex?\sum_{i=1}^{c}\sum_{i=1}^{c}K[i,j]%20=%201)

Operacja filtrowania tworzy nowy obraz J, którego piksele mają wartość:
![equation](https://latex.codecogs.com/gif.latex?J[x,y]=round(s_{x,y})),
![equation](https://latex.codecogs.com/gif.latex?s_{x,y}%20=%20\sum_{i=1}^{c}\sum_{i=1}^{c}I[max\left{1,%20x%20-%20ceil(c/2)%20+%20i\right},%20max\left{1,%20y%20-%20ceil(c/2)%20+%20j\right}]%20\cdot%20k[i,j].)
Operacja  round() oznacza zaokrąglenie do najbliższej liczby całkowitej a ceil() zaokrąglenie w górę do najbliższej liczby całkowitej.
Zwróć uwagę, że w powyższym opisie przyjęto matematyczną konwencję indeksowania elementów macierzy - od indeksu 1.
## Zadanie 1
Napisz program, który wykonuje wielowątkową operację filtrowania obrazu. Program przyjmuje w argumentach wywołania:
1. liczbę wątków,
2. nazwę pliku z wejściowym obrazem,
3. nazwę pliku z definicją filtru,
4. nazwę pliku wynikowego.

Po wczytaniu danych (wejściowy obraz i definicja filtru) wątek główny tworzy tyle nowych wątków, ile zażądano w argumencie wywołania. Utworzone wątki równolegle generują wyjściowy (filtrowany) obraz. Każdy wątek odpowiada za wygenerowanie fragmentu wyniku - np. generuje pionowy pasek o szerokości *N/k* pikseli, gdzie *N* to szerokość wyjściowego obrazu a *k* to liczba wątków. Wątek główny czeka na zakończenie pracy przez wątki wykonujące operację filtrowania. Następnie zapisuje powstały obraz do pliku wynikowego. Dodatkowo wątek główny mierzy czas rzeczywisty operacji filtrowania i wypisuje go na ekranie. W mierzonym czasie należy uwzględnić narzut związany z utworzeniem i zakończeniem wątków (ale bez czasu operacji wejścia/wyjścia).
Wykonaj pomiary czasu operacji filtrowania dla obrazu o rozmiarze kilkaset na kilkaset pikseli i kilku filtrów (można wykorzystać losowe macierze filtrów). Testy przeprowadź dla 1, 2, 4, i 8 wątków. Rozmiar filtrów dobierz w zakresie 3≤c≤65, tak aby uwidocznić wpływ liczby wątków na czas operacji filtrowania . Wyniki zamieść w pliku *Times.txt* i dołącz do archiwum z rozwiązaniem zadania.
#### Format wejścia-wyjścia
Program powinien odczytywać i zapisywać obrazy w formacie ASCII PGM (Portable Gray Map). Pliki w tym formacie mają nagłówek postaci:

P2
W H
M
...

gdzie: W to szerokość obrazu w pikselach, H to wysokość obrazu w pikselach a M to maksymalna wartość piksela. Zakładamy, że obsługujemy jedynie obrazy w 256 odcieniach szarości: od 0 do 255 (a więc M=255). Po nagłówku, w pliku powinno być zapisanych W*H liczb całkowitych reprezentujących wartości kolejnych pikseli. Liczby rozdzielone są białymi znakami (np. spacją). Piksele odczytywane są wierszami, w kolejności od lewego górnego do prawego dolnego rogu obrazu.

Przykładowe obrazy w formacie ASCII PGM (jak również opis formatu) można znaleźć pod adresem: http://people.sc.fsu.edu/~jburkardt/data/pgma/pgma.html

W pierwszej linii pliku z definicją filtru powinna znajdować się liczba całkowita c określająca rozmiar filtru. Dalej, plik powinien zawierać c*c liczb zmiennoprzecinkowych określających wartości elementów filtru (w kolejności wierszy, od elementu K[1,1] do elementu K[c,c]).
