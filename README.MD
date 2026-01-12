# Kody Huffmana - Projekt Rafał Jankowski

### Struktura nagłówka i tablicy danych

```cpp
std::byte magic_bytes[4] // Magiczne bity HUFF         [4B]
uint64_t length          // Długość orginalnych danych [8B]
uint16_t count           // Ilość różnych bitów        [2B]
// Dla każdego bitu:
  std::byte  byte        //                            [1B]
  uint32_t frequency     //                            [4B]
std::byte Data[len]      // Dane długości len          []
```
## Użycie
### Komendy aplikacji terminala
Krótke przedstawienie opcji aplikacji terminala, oraz przykładowe użycie.<br>
```-c [path_to_target]``` Compress - Opcja kompresji pliku danego w argumencie opcji<br>
```-d [path_to_target]``` Decompress - Opcja dekompresji pliku danego w argumencie opcji<br>
```-o [nazwa_pliku_wyjściowego]``` Output - Razem z opcją -c pozwala wybrać nazwę pliku wyjściowego kompresji

przykładowe użycie:<br>
```huff -c moj_plik.txt```
takie użycie utworzy w lokalizacji w której leży aplikacja skompresowane dane z orginalnego pliku. Utworzony plik będzie miał rozszerzenie ".huff"