cmake_minimum_required(VERSION 3.14)
project(oop_lab6)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Включаем все заголовочные файлы
include_directories(include)

# Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.15.2
  TLS_VERIFY false
)
FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE CPP_SOURCES "src/*.cpp")

add_executable(lab6
    main.cpp
    ${CPP_SOURCES}
)

add_executable(tests 
    test/test_point.cpp
    test/test_npc.cpp
    test/test_factory.cpp
    test/test_dungeon.cpp
    test/test_battle.cpp 
    test/test_observer.cpp
    test/test_game.cpp
    ${CPP_SOURCES}  
)

target_include_directories(tests PRIVATE include)
target_link_libraries(tests gtest gtest_main)

enable_testing()
add_test(NAME oop_lab6_tests COMMAND tests)