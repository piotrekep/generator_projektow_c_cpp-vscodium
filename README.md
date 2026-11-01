# VSCodium C/C++ CMake templates v3

Generator prostych projektów C i C++ dla VSCodium na Linuksie.

Zestaw zawiera:

- szablon C17,
- szablon C++20,
- CMake + Ninja,
- clangd,
- autocomplete,
- parameter hints,
- automatyczne dodawanie `#include`,
- Debug / Release / Profile / ASan+UBSan,
- opcjonalne testy Criterion,
- debugowanie przez CodeLLDB,
- narzędzia do profilowania i analizy programu.

## Struktura

```text
vscodium-cmake-templates-v3/
├── c-cmake/
├── cpp-cmake/
├── newproj
└── README.md
```

Przykładowy projekt C++ z testami:

```text
my_project/
├── CMakeLists.txt
├── CMakePresets.json
├── include/
│   └── app.hpp
├── src/
│   ├── app.cpp
│   └── main.cpp
├── tests/
│   └── test_app.cpp
└── .vscode/
    ├── launch.json
    ├── settings.json
    └── tasks.json
```

Logika programu jest budowana jako biblioteka `${PROJECT_NAME}_lib`. Dzięki temu ten sam kod można linkować do programu głównego i testów Criterion.

## Wymagane pakiety

### Arch Linux

```bash
sudo pacman -S --needed \
    base-devel \
    cmake \
    ninja \
    clang \
    gdb \
    git \
    pkgconf \
    criterion \
    perf \
    valgrind \
    kcachegrind \
    heaptrack \
    strace
```

`clang` dostarcza m.in. `clangd`, `clang-format` i `clang-tidy`.

Hotspot może wymagać AUR lub innego źródła pakietów, jeśli nie jest dostępny w aktywnych repozytoriach.

### Debian

```bash
sudo apt install \
    build-essential \
    cmake \
    ninja-build \
    clangd \
    gdb \
    git \
    pkg-config \
    libcriterion-dev \
    linux-perf \
    valgrind \
    kcachegrind \
    heaptrack \
    strace
```

## Rozszerzenia VSCodium

Zainstaluj:

- `clangd`
- `CMake Tools`
- `CodeLLDB`

Opcjonalnie:

- GitLens

## Instalacja generatora

Przykład:

```bash
mkdir -p ~/projects/templates
cp -a c-cmake cpp-cmake newproj ~/projects/templates/
chmod +x ~/projects/templates/newproj
```

Możesz utworzyć symlink:

```bash
mkdir -p ~/bin
ln -s ~/projects/templates/newproj ~/bin/newproj
```

`newproj` rozwiązuje symlink przez `readlink -f`, więc szablony mogą pozostać w katalogu `~/projects/templates`.

Jeżeli `~/bin` nie jest w `PATH`, dodaj do `~/.bashrc`:

```bash
export PATH="$HOME/bin:$PATH"
```

Potem:

```bash
source ~/.bashrc
```

## Tworzenie projektu

C++ bez testów:

```bash
newproj cpp test_cpp ~/projects
```

C z Criterion:

```bash
newproj c test_c ~/projects --criterion
```

C++ z Criterion:

```bash
newproj cpp test_cpp ~/projects --criterion
```

Możesz pominąć katalog docelowy:

```bash
newproj cpp test_cpp --criterion
```

Wtedy projekt zostanie utworzony w bieżącym katalogu.

Generator:

- kopiuje odpowiedni szablon,
- zastępuje `@PROJECT_NAME@` nazwą projektu,
- nie modyfikuje zmiennej CMake `${PROJECT_NAME}`,
- opcjonalnie włącza Criterion,
- wykonuje `git init`.

## Build

### Debug

```bash
cmake --preset debug
cmake --build --preset debug
```

W VSCodium:

```text
Ctrl+Shift+B
```

### Release

```bash
cmake --preset release
cmake --build --preset release
```

### Profile

```bash
cmake --preset profile
cmake --build --preset profile
```

Preset `profile` używa:

```text
RelWithDebInfo
-fno-omit-frame-pointer
```

Dzięki temu program jest zoptymalizowany, zachowuje symbole debugowe i nadaje się do profilowania stosu.

### ASan + UBSan

```bash
cmake --preset asan
cmake --build --preset asan
```

Preset włącza:

- AddressSanitizer,
- UndefinedBehaviorSanitizer,
- `-fno-omit-frame-pointer`.

## clangd

Szablon zawiera:

```json
"clangd.arguments": [
    "--background-index",
    "--clang-tidy",
    "--completion-style=detailed",
    "--compile-commands-dir=build/debug",
    "--header-insertion=iwyu"
]
```

Po:

```bash
cmake --preset debug
```

powstaje:

```text
build/debug/compile_commands.json
```

clangd używa go do:

- autocomplete,
- analizy `#include`,
- goto definition,
- find references,
- rename symbol,
- parameter hints,
- automatycznego dodawania nagłówków,
- clang-tidy.

Jeżeli clangd nie odświeży konfiguracji po pierwszym configure:

```text
Ctrl+Shift+P
clangd: Restart language server
```

## Criterion

Przykładowy test:

```cpp
#include <criterion/criterion.h>
#include "app.hpp"

Test(app, addition)
{
    cr_assert_eq(add(2, 3), 5);
}
```

Kilka testów może znajdować się w jednym pliku:

```cpp
Test(app, addition)
{
    cr_assert_eq(add(2, 3), 5);
}

Test(app, negative_values)
{
    cr_assert_eq(add(-2, 3), 1);
}

Test(app, zero)
{
    cr_assert_eq(add(0, 0), 0);
}
```

Możesz też używać wielu plików:

```text
tests/
├── test_app.cpp
├── test_parser.cpp
└── test_protocol.cpp
```

Dodaj je do istniejącego targetu:

```cmake
add_executable(${PROJECT_NAME}_tests
    tests/test_app.cpp
    tests/test_parser.cpp
    tests/test_protocol.cpp
)
```

Nie trzeba tworzyć osobnego `add_test()` dla każdego `Test(...)` Criterion. CTest widzi jeden executable testowy, a Criterion uruchamia wszystkie test cases wewnątrz niego.

Uruchomienie testów:

```bash
ctest --test-dir build/debug --output-on-failure
```

Verbose:

```bash
ctest --test-dir build/debug -V
```

Bezpośrednio:

```bash
./build/debug/<nazwa_projektu>_tests
```

## Debugowanie przez CodeLLDB

`launch.json` zawiera:

```text
CodeLLDB: Debug
CodeLLDB: Profile build
```

W VSCodium wybierz `Run and Debug` i uruchom `CodeLLDB: Debug`.

Przed startem debugger wykonuje task:

```text
CMake: Build Debug
```

Program debugowy:

```text
build/debug/<nazwa_projektu>
```

## Zadania VSCodium

Otwórz:

```text
Terminal -> Run Task
```

### perf

CPU, instrukcje, cache i branch prediction:

```text
Profile: perf stat CPU/cache
```

Mierzone są m.in.:

- cycles,
- instructions,
- cache-references,
- cache-misses,
- branches,
- branch-misses.

Nagranie profilu:

```text
Profile: perf record
```

Raport tekstowy:

```text
Profile: perf report
```

Hotspot:

```text
Profile: Hotspot
```

Wynik:

```text
build/profiling/perf.data
```

## Valgrind

### Memcheck

```text
Valgrind: Memcheck
```

Analizuje m.in.:

- wycieki pamięci,
- niepoprawne dostępy,
- użycie niezainicjalizowanej pamięci.

### Callgrind

```text
Valgrind: Callgrind
```

Wynik:

```text
build/profiling/callgrind.out
```

GUI:

```text
Valgrind: KCachegrind
```

### Cachegrind

```text
Valgrind: Cachegrind
```

Analiza tekstowa:

```text
Valgrind: Cachegrind annotate
```

Cachegrind służy do analizy zachowania cache i liczby wykonywanych instrukcji.

## Heaptrack

Nagranie:

```text
Heaptrack: Record
```

GUI:

```text
Heaptrack: GUI
```

Heaptrack pokazuje m.in.:

- liczbę alokacji,
- ilość zaalokowanej pamięci,
- szczytowe użycie pamięci,
- stosy odpowiedzialne za alokacje.

## strace

Podsumowanie syscalli:

```text
Trace: strace summary
```

Pełny zapis:

```text
Trace: strace full
```

Wynik:

```text
build/profiling/strace.log
```

## GDB

Interaktywne uruchomienie:

```text
GDB: Run
```

Analiza core dumpa:

```text
GDB: Backtrace core
```

Task poprosi o ścieżkę do pliku core i wykona pełny backtrace wszystkich wątków.

## Katalog wyników profilowania

```text
build/profiling/
├── perf.data
├── callgrind.out
├── cachegrind.out
├── heaptrack*.gz
└── strace.log
```

Katalog `build/` jest ignorowany przez Git.

## Typowy workflow

Development:

```text
Debug
  -> clangd
  -> CodeLLDB
  -> Criterion
```

Problemy z pamięcią i UB:

```text
ASan / UBSan
Valgrind Memcheck
```

Wydajność CPU:

```text
Profile
  -> perf stat
  -> perf record
  -> Hotspot
```

Call graph:

```text
Callgrind
  -> KCachegrind
```

Cache:

```text
Cachegrind
```

Alokacje:

```text
Heaptrack
```

Syscalls i I/O:

```text
strace
```
