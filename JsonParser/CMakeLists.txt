cmake_minimum_required(VERSION 3.17)
project(JsonParser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Флаги компиляции
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -O2")

# Определяем путь к папке lib
set(LIB_DIR ${CMAKE_SOURCE_DIR}/lib)

# Проверяем наличие файлов SQLite3 в папке lib
if(EXISTS ${LIB_DIR}/sqlite3.h AND EXISTS ${LIB_DIR}/sqlite3.c)
    message(STATUS "Найден SQLite3 в папке lib")

    # Добавляем SQLite3 как статическую библиотеку
    add_library(sqlite3 STATIC ${LIB_DIR}/sqlite3.c)

    # Устанавливаем путь к заголовочным файлам SQLite3
    target_include_directories(sqlite3 PUBLIC ${LIB_DIR})

    # Устанавливаем флаги компиляции для SQLite3
    target_compile_definitions(sqlite3 PRIVATE
            -DSQLITE_THREADSAFE=1
            -DSQLITE_USE_URI=1
            -DSQLITE_ENABLE_COLUMN_METADATA=1
            -DSQLITE_ENABLE_FTS5=1
            -DSQLITE_ENABLE_RTREE=1
            -DSQLITE_ENABLE_JSON1=1
            )

    # Если компилятор GCC или Clang, добавляем флаги
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(sqlite3 PRIVATE -w) # Отключаем предупреждения для SQLite
    endif()

else()
    message(FATAL_ERROR "SQLite3 не найден в папке lib. Пожалуйста, поместите sqlite3.h и sqlite3.c в папку ${LIB_DIR}")
endif()

# Включаем директории проекта
include_directories(${CMAKE_SOURCE_DIR}/include)

# Основная программа
add_executable(json_parser
        src/main.cpp
        src/ConsoleInterface.cpp
        src/JsonStreamProcessor.cpp
        src/JsonObjectExtractor.cpp
        src/JsonParser.cpp
        src/JsonNode.cpp
        src/JsonDatabase.cpp
        src/BufferedJsonReader.cpp
        )

# Линкуем с нашей библиотекой SQLite3
target_link_libraries(json_parser PRIVATE sqlite3)

# Для Windows нужно линковать с дополнительными библиотеками
if(WIN32)
    target_link_libraries(json_parser PRIVATE ws2_32)
endif()

# Устанавливаем свойства для целевого файла
set_target_properties(json_parser PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        )

# Копирование SQLite3 файлов в build (опционально)
configure_file(${LIB_DIR}/sqlite3.h ${CMAKE_BINARY_DIR}/sqlite3.h COPYONLY)
configure_file(${LIB_DIR}/sqlite3.c ${CMAKE_BINARY_DIR}/sqlite3.c COPYONLY)

# Тесты (опционально)
option(BUILD_TESTS "Build tests" OFF)

if(BUILD_TESTS)
    enable_testing()

    # Ищем Google Test
    find_package(GTest QUIET)

    if(GTest_FOUND)
        message(STATUS "Google Test найден, собираем тесты")

        add_executable(json_tests
                tests/test_json_parser.cpp
                tests/test_json_node.cpp
                tests/test_json_extractor.cpp
                src/JsonParser.cpp
                src/JsonNode.cpp
                src/JsonObjectExtractor.cpp
                src/BufferedJsonReader.cpp
                )

        target_include_directories(json_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
        target_link_libraries(json_tests PRIVATE sqlite3 GTest::gtest GTest::gtest_main)

        if(WIN32)
            target_link_libraries(json_tests PRIVATE ws2_32)
        endif()

        add_test(NAME JsonParserTests COMMAND json_tests)

    else()
        message(WARNING "Google Test не найден, тесты не будут собраны")
    endif()
endif()