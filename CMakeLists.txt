cmake_minimum_required(VERSION 3.10)
project(Lochy CXX)

# Ustawienie standardu języka na C++20 zgodnie z założeniami projektu
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dodanie wszystkich plików źródłowych Twojej gry
add_executable(Lochy
    main.cpp
    game.cpp
    game_map.cpp
    game_render.cpp
    game_entities.cpp
)

# Podpięcie biblioteki raylib do projektu
target_link_libraries(Lochy PRIVATE raylib)