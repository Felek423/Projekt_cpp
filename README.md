# Lochy – 2D Roguelike

Projekt akademicki gry typu Roguelike z widokiem top-down, mocno inspirowanej klasykami gatunku takimi jak *The Binding of Isaac*. Gra oferuje proceduralnie generowane lochy, mechanikę permadeath (ostateczna śmierć) oraz dynamiczny system walki z przeciwnikami i ostatecznym Bossem.

##  Funkcjonalności

- **Proceduralna Generacja:** Za każdym uruchomieniem gry układ lochów (do 10 połączonych pokoi) oraz rozmieszczenie przeszkód i wrogów generowane są od zera.
- **Zróżnicowani Przeciwnicy:** Kilka typów wrogów posiadających odmienne statystyki i wzorce ataków (strzelanie kierunkowe, strzelanie obszarowe, pogoń za graczem) oraz unikalny Boss.
- **Sztuczna Inteligencja (AI):** Przeciwnicy używają algorytmów śledzenia gracza opartych na wektorach kierunkowych. Posiadają również system omijania przeszkód ("ślizgania się" po skałach) oraz wzajemnego odpychania, zapobiegający nakładaniu się na siebie hitboxów.
- **System Pickups:** W skrzyniach i odblokowanych pokojach na gracza czekają przedmioty leczące oraz przyspieszające atak (zwiększające szybkostrzelność).
- **Interfejs Użytkownika:** Pasek zdrowia w formie serc, dynamicznie rysowana minimapa pozwalająca na orientację w układzie lochów oraz pasek życia Bossa.
- **Warstwa Audio:** Asynchroniczne strumieniowanie muzyki grającej płynnie w tle, niezależnie od pętli głównej i pauz.

## 💻 Technologie

- **Język:** C++20
- **Biblioteka:** Raylib (obsługa grafiki 2D, inputu oraz audio)
- **Build system:** CMake

##  Architektura Kodu (Struktura plików)

Projekt został podzielony na logiczne moduły, aby zachować czystość i czytelność:

- `main.cpp` - Główna pętla programu, inicjalizacja okna, strumieniowanie muzyki oraz zarządzanie globalnym stanem pauzy.
- `game.h` / `game.cpp` - Definicja i inicjalizacja głównej klasy `Game`. Ładowanie zasobów z dysku (tekstury, czyszczenie pamięci po zakończeniu).
- `types.h` - Definicje kluczowych struktur danych grupujących zmienne (np. `bullet`, `enemy`, `pickup`, `RoomData`).
- `game_map.cpp` - Algorytmy proceduralnego generowania mapy lochów (odgałęzienia i drzwi) oraz układu przeszkód (wycinanie bezpiecznych ścieżek "na krzyż"). Obsługa przechodzenia między planszami.
- `game_entities.cpp` - Aktualizacja logiki: matematyka ruchu gracza, AI i omijanie martwych punktów przez przeciwników, strzelanie, sprawdzanie kolizji i bezpieczne niszczenie wektorów (`std::vector`).
- `game_render.cpp` - Funkcje renderujące: rysowanie tekstur i kształtów (metody `Draw`), menu pauzy, ekran końcowy (Game Over) oraz UI.

##  Sterowanie

- **W, A, S, D** - Poruszanie się postacią
- **Strzałki (Góra, Dół, Lewo, Prawo)** - Strzelanie w 4 kierunkach niezależnie od kierunku ruchu
- **ESC** - Pauza gry / Menu
- **Lewy Przycisk Myszy** - Interakcja z przyciskami w menu i na ekranie Game Over

##  Kompilacja i uruchomienie (Linux/macOS)

Projekt korzysta z systemu CMake. Zalecane jest budowanie typu *out-of-source* (w oddzielnym folderze `build`).

### Wymagania
- Kompilator C++ obsługujący standard C++20 (np. GCC / Clang)
- Zainstalowana biblioteka `raylib` (w Ubuntu/Debian: `sudo apt install libraylib-dev`)
- CMake

### Instrukcja budowania

Otwórz terminal w katalogu z projektem i wykonaj poniższe kroki:

```bash
# 1. Utworzenie katalogu na pliki binarne
mkdir build
cd build

# 2. Wygenerowanie konfiguracji
cmake ..

# 3. Kompilacja gry
make

# 4. Uruchomienie
./Lochy
```

## 🧠 Ciekawostki inżynieryjne

1. **Omijanie "Martwego Punktu" w AI:** Podczas projektowania przeciwników napotkano na problem zacinania się wroga bezpośrednio za przeszkodą w prostej linii do gracza. Rozwiązano to implementując wektor poślizgu (`slideForce`) i łamanie balansu poprzez badanie iloczynu skalarnego – wymusza to zrzucenie wroga z idealnego środka przeszkody i płynne jej ominięcie.
2. **Zarządzanie Pamięcią i Wektorami:** Liczba pocisków, przedmiotów i przeciwników w grze nieustannie ulega zmianie. By zapobiec błędom wyjścia poza tablicę (segfault) oraz wyciekom, czyszczenie wektorów używając `erase()` odbywa się w pętlach iterujących *od tyłu* (od indeksu `size() - 1` do `0`). 
3. **Ochrona przed zablokowaniem:** Algorytm rozmieszczający kamienie w pokoju losuje ich koordynaty z ograniczeniem – wycina w siatce krzyż gwarantujący bezpośrednią, prostą drogę do każdych z czterech możliwych drzwi. Dzięki temu upewniamy się, że generator nigdy nie stworzy "pułapki" bez wyjścia.