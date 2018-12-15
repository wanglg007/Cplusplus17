# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)
# 使用C++17
set(CMAKE_CXX_STANDARD 17)
# 项目信息
project (My_Cplusplus17)
# 指定boost
set (Boost_USE_STATIC_LIBS ON)  # enable dynamic linking
set (Boost_USE_MULTITHREAD ON)  # enable multithreading
# windows 下设置这两个路径，find_package才能找到Boost
SET(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "D:/Development_Install/Library/boost_1_68_0")
SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "D:/Development_Install/Library/boost_1_68_0/stage/lib")
#
find_package(Boost)
if(NOT Boost_FOUND)
    message("not found boost package")
else()
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
    LINK_DIRECTORIES(${CMAKE_LIBRARY_PATH})
endif()
# 指定生成目标
add_executable(main main.cpp)
####################(2)concurrency####################
##########example00##########
#add_executable(Ex00_01 02_concurrency/example00/Ex00_01.cpp)
#add_executable(Ex00_02 02_concurrency/example00/Ex00_02.cpp)
#add_executable(Ex00_03 02_concurrency/example00/Ex00_03.cpp)
#add_executable(Ex00_04 02_concurrency/example00/Ex00_04.cpp)
#add_executable(Ex00_05 02_concurrency/example00/Ex00_05.cpp)
#add_executable(Ex00_06 02_concurrency/example00/Ex00_06.cpp)
#add_executable(Ex00_07 02_concurrency/example00/Ex00_07.cpp)
#add_executable(Ex00_08 02_concurrency/example00/Ex00_08.cpp)
#add_executable(Ex00_09 02_concurrency/example00/Ex00_09.cpp)
##########example01##########
#add_executable(Ex01_01 02_concurrency/example01/Ex01_01.cpp)
#add_executable(Ex01_02 02_concurrency/example01/Ex01_02.cpp)
#add_executable(Ex01_03 02_concurrency/example01/Ex01_03.cpp)
#add_executable(Ex01_04 02_concurrency/example01/Ex01_04.cpp)
#add_executable(Ex01_05 02_concurrency/example01/Ex01_05.cpp)
#add_executable(Ex01_06 02_concurrency/example01/Ex01_06.cpp)
#add_executable(Ex01_07 02_concurrency/example01/Ex01_07.cpp)
#add_executable(Ex01_08 02_concurrency/example01/Ex01_08.cpp)
#add_executable(Ex01_09 02_concurrency/example01/Ex01_09.cpp)
#add_executable(Ex01_10 02_concurrency/example01/Ex01_10.cpp)
#add_executable(Ex01_11 02_concurrency/example01/Ex01_11.cpp)
#add_executable(Ex01_12 02_concurrency/example01/Ex01_12.cpp)
##########example02##########
add_executable(Ex02_01 02_concurrency/example02/Ex02_01.cpp)
add_executable(Ex02_02 02_concurrency/example02/Ex02_02.cpp)
add_executable(Ex02_03 02_concurrency/example02/Ex02_03.cpp)
add_executable(Ex02_04 02_concurrency/example02/Ex02_04.cpp)
add_executable(Ex02_05 02_concurrency/example02/Ex02_05.cpp)