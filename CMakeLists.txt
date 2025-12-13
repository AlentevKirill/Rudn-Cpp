cmake_minimum_required(VERSION 3.10)
project(BankingSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ВАЖНО: указываем директории для заголовочных файлов
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/account
        ${CMAKE_CURRENT_SOURCE_DIR}/include/bank
        ${CMAKE_CURRENT_SOURCE_DIR}/include/client
        ${CMAKE_CURRENT_SOURCE_DIR}/include/exception
        ${CMAKE_CURRENT_SOURCE_DIR}/include/transaction
        ${CMAKE_CURRENT_SOURCE_DIR}/include/util
)

# Все исходные файлы
set(SOURCES
        src/main.cpp
        src/account/Account.cpp
        src/bank/Bank.cpp
        src/client/Client.cpp
        src/client/VipClient.cpp
        src/exception/BankException.cpp
        src/transaction/Transaction.cpp
        src/util/Date.cpp
        src/util/Address.cpp
        )

# Создаем исполняемый файл
add_executable(${PROJECT_NAME} ${SOURCES})