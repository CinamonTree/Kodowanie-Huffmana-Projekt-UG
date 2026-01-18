# Kody Huffmana - Projekt Rafał Jankowski
Projekt na zajęcia "Algorytmy i struktury danych" na Uniwersytecie Gdańskim.
## 1. Użycie
### 1.1 Lokalizacja plików wykonywalnych
Skompilowane pliki znajdują się w folderze "bin" głównego katalogu.<br>
Plik **priority_queue_win_x86_64.exe** zawiera konsolową aplikację z interfejsem przedstawiającą działanie implementacji kolejki priorytetowej.<br>
Plik **huff_win_x86_64.exe** zawiera aplikację<br>
### 1.2 Kolejka priorytetowa
Pierwsza część projektu zawiera kolejkę priorytetową w postaci aplikacji terminala z czytelnym prostym interfejsem.<br>
Użycie wygląda w następujący sposób:
1. Uruchamiamy aplikację w dowolnym miejscu.
2. Po wyświetleniu ekranu tytułowego naciskamy Enter aby przejść dalej.
3. Wybieramy typ danych na jakich będzie działać kolejka. Do wyboru:
    - INT
    - CHAR
    - STRING
4. Po wybraniu typu mamy operacje.
    - POP - Które usuwa element o najniższym priorytecie.
    - PUSH - Po podaniu priorytetu i danych wrzuca element do kopca i go odbudowuje.
    - EXIT - Kończy działanie aplikacji.
### 1.3 Aplikacja Kompresji Kodami Huffmana
Aplikacja **huff_win_x86_64.exe** pozwala na kompresję dowolnego rodzaju pliku do formatu .huff własnej implementacji, a także późniejszą dekompresję.<br>
Z aplikacji korzysta się w stylu komend UNIX-owych. W dowolnym terminalu wpisujemy nazwę aplikacji następując opcjami i ich argumentami. Przykład przedstawiony poniżej.<br>
Krótkie przedstawienie opcji aplikacji terminala, oraz przykładowe użycie.<br>
```huff -c [ścieżka_pliku]``` Compress - Opcja kompresji pliku danego w argumencie opcji. Kompresuje plik do folderu roboczego.<br>
```huff -d [ścieżka_pliku]``` Decompress - Opcja dekompresji pliku danego w argumencie opcji. Plik który chcemy zdekompresować musi mieć rozszerzenie .huff.<br>
```huff -o [ścieżka_wyjścia]``` Output - Razem z opcją -c | -d pozwala wybrać docelowe miejsce wyjścia.<br>
Przykładowe użycie:<br>
```huff -c moj_plik.txt -o ./kompresje/moj_skompresowany_plik.txt```
Takie użycie utworzy w podanej lokalizacji pliku wyjściowego plik o nazwie *moj_skompresowany_plik.txt.huff*. Ważne jest aby podać w opcji -o rozszerzenie pliku aby nie zgubić go przy dekompresji.<br>
```huff -d ./kompresje/moj_skompresowany_plik.txt.huff```
Takie użycie natomiast utworzy w folderze roboczym plik o nazwie *moj_skompresowany_plik.txt*, który będzie defacto dekompresją pliku podanego w argumencie opcji -d.
## 2. Implementacja
### 2.2 Podział na dwie części
Projekt jest podzielony na dwie części dla lepszej czytelności.<br>
Pierwsza część jest to aplikacja konsolowa z własnym interfejsem dla przetestowania kolejki.<br>
Druga część jest to właściwy projekt kodowania huffmana która korzysta z implementacji wcześniej napisanej kolejki priorytetowej.<br>
### 2.3 Struktura nagłówka i tablicy danych
Nagłówek jest zapisywany jako tekstowe linie w formacie `symbol:czestotliwosc`,
po jednej linii na każdy symbol. Symbol jest kodowany znakami specjalnymi
(`\\n`, `\\r`, `\\t`, `\\s`, `\\:`, `\\\\`) lub w postaci `\\xHH` dla bajtów
niedrukowalnych. Nagłówek kończy pusta linia. Po nim występuje strumień bitów
danych, a na samym końcu zapisywany jest jeden bajt z liczbą bitów paddingu
(0-7) dla ostatniego bajtu danych.
Przykład linii nagłówka:
```
\x0A:15
a:120
\::3
```