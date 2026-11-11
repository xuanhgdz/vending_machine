# Yeu cau phien ban CMake toi thieu
cmake_minimum_required(VERSION 3.10)

# Dat ten cho project
project(VendingMachineOOP)

# Thiet lap chuan C++ (ho tro C++17 / C++20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Tim tat ca cac file .cpp trong thu muc src
file(GLOB SOURCES "src/*.cpp")

# Tao file thuc thi chinh tu main.cpp va cac file trong thu muc src
add_executable(app main.cpp ${SOURCES})

# Khai bao duong dan den thu muc include de trinh bien dich tim thay cac file .h
target_include_directories(app PRIVATE include)

# Tao file thuc thi kiem thu tu dong (tests/test_scenarios.cpp)
add_executable(run_tests tests/test_scenarios.cpp ${SOURCES})
target_include_directories(run_tests PRIVATE include)
