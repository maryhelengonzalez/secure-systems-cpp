cmake_minimum_required(VERSION 3.16)

project(secure_systems_cpp
    VERSION 1.0
    DESCRIPTION "Advanced C++ systems, concurrency, and security simulations"
    LANGUAGES CXX
)

# Use modern C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable warnings
if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Include directories
include_directories(include)

# Main application
add_executable(app
    src/main.cpp
    src/ThreadManager.cpp
)

# Threading support
find_package(Threads REQUIRED)
target_link_libraries(app PRIVATE Threads::Threads)

# Test executable
add_executable(test_threadmanager
    tests/test_threadmanager.cpp
    src/ThreadManager.cpp
)

target_link_libraries(test_threadmanager PRIVATE Threads::Threads)

# --- Banking Core ---
add_library(bank_core
  src/bank/Bank.cpp
   src/bank/Ledger.cpp

)
target_include_directories(bank_core PUBLIC include)
target_compile_features(bank_core PUBLIC cxx_std_17)
target_link_libraries(bank_core PUBLIC pthread)

# --- Bank Tests ---
add_executable(test_bank
  tests/bank/test_bank.cpp
)
target_link_libraries(test_bank PRIVATE bank_core)
