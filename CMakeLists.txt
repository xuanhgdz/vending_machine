cmake_minimum_required(VERSION 3.16)
project(VendingMachineWeek1 VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(vending_core
    src/Money.cpp
    src/Product.cpp
    src/Slot.cpp
    src/Inventory.cpp
    src/MachineState.cpp
    src/VendingMachine.cpp
    src/DemoData.cpp
)
target_include_directories(vending_core PUBLIC include)

add_executable(vending_app src/main.cpp src/ConsoleUI.cpp)
target_link_libraries(vending_app PRIVATE vending_core)

function(vending_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive- /utf-8)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
endfunction()
vending_warnings(vending_core)
vending_warnings(vending_app)

include(CTest)
if(BUILD_TESTING)
    add_executable(vending_tests tests/test_main.cpp)
    target_link_libraries(vending_tests PRIVATE vending_core)
    vending_warnings(vending_tests)
    add_test(NAME core_contracts COMMAND vending_tests)

    find_package(Python3 COMPONENTS Interpreter QUIET)
    if(Python3_Interpreter_FOUND)
        add_test(NAME console_scenarios
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_console.py
                $<TARGET_FILE:vending_app> ${CMAKE_CURRENT_SOURCE_DIR}/data)
    endif()
endif()
