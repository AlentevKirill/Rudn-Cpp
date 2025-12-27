cmake_minimum_required(VERSION 3.17)
project(JsonParser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Флаги компиляции
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -O2")


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
        src/JsonStreamReader.cpp
        src/BufferedJsonReader.cpp
        )

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
                src/JsonStreamReader.cpp
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