# CMake 最低版本号要求
cmake_minimum_required(VERSION 2.8)
# 使用C++17
set(CMAKE_CXX_STANDARD 17)
# 项目信息
project(My_Cplusplus17)
# 指定boost
set(Boost_USE_STATIC_LIBS ON)  # enable dynamic linking
set(Boost_USE_MULTITHREAD ON)  # enable multithreading
# windows 下设置这两个路径，find_package才能找到Boost
SET(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "D:/Development_Install/Library/boost_1_68_0")
SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "D:/Development_Install/Library/boost_1_68_0/stage/lib")
#
find_package(Boost)
if (NOT Boost_FOUND)
    message("not found boost package")
else ()
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
    LINK_DIRECTORIES(${CMAKE_LIBRARY_PATH})
endif ()
# 指定生成目标
add_executable(main main.cpp)
####################(3)network####################
##########example00##########
#add_executable(Ex00_01 03_network/example00/Ex00_01.cpp)
#add_executable(Ex00_02 03_network/example00/Ex00_02.cpp)
#add_executable(Ex00_03 03_network/example00/Ex00_03.cpp)
#add_executable(Ex00_04 03_network/example00/Ex00_04.cpp)
#add_executable(Ex00_05 03_network/example00/Ex00_05.cpp)
#add_executable(Ex00_06 03_network/example00/Ex00_06.cpp)
#add_executable(Ex00_07 03_network/example00/Ex00_07.cpp)
#add_executable(Ex00_08 03_network/example00/Ex00_08.cpp)
#add_executable(Ex00_09 03_network/example00/Ex00_09.cpp)
#add_executable(Ex00_10 03_network/example00/Ex00_10.cpp)
#add_executable(Ex00_11 03_network/example00/Ex00_11.cpp)
#add_executable(Ex00_12 03_network/example00/Ex00_12.cpp)
#add_executable(Ex00_13 03_network/example00/Ex00_13.cpp)
#add_executable(Ex00_14 03_network/example00/Ex00_14.cpp)
#add_executable(Ex00_15 03_network/example00/Ex00_15.cpp)
#add_executable(Ex00_16 03_network/example00/Ex00_16.cpp)
#add_executable(Ex00_17 03_network/example00/Ex00_17.cpp)
#add_executable(Ex00_18 03_network/example00/Ex00_18.cpp)
#add_executable(Ex00_19 03_network/example00/Ex00_19.cpp)
#add_executable(Ex00_20 03_network/example00/Ex00_20.cpp)
#add_executable(Ex00_21 03_network/example00/Ex00_21.cpp)
#add_executable(Ex00_22 03_network/example00/Ex00_22.cpp)
#add_executable(Ex00_23 03_network/example00/Ex00_23.cpp)
#add_executable(Ex00_24 03_network/example00/Ex00_24.cpp)
#add_executable(Ex00_25 03_network/example00/Ex00_25.cpp)
#add_executable(Ex00_26 03_network/example00/Ex00_26.cpp)
#add_executable(Ex00_27 03_network/example00/Ex00_27.cpp)
#add_executable(Ex00_28 03_network/example00/Ex00_28.cpp)
#add_executable(Ex00_29 03_network/example00/Ex00_29.cpp)
#add_executable(Ex00_30 03_network/example00/Ex00_30.cpp)
##########example01##########
#add_executable(Ex00_01 03_network/example01/Ex00_01.cpp)
#add_executable(Ex00_02 03_network/example01/Ex00_02.cpp)
#add_executable(Ex00_03 03_network/example01/Ex00_03.cpp)
#add_executable(Ex00_04 03_network/example01/Ex00_04.cpp)
#add_executable(Ex00_05 03_network/example01/Ex00_05.cpp)
#add_executable(Ex00_06 03_network/example01/Ex00_06.cpp)
#add_executable(Ex00_07 03_network/example01/Ex00_07.cpp)
#add_executable(Ex00_08 03_network/example01/Ex00_08.cpp)
#add_executable(Ex00_09 03_network/example01/Ex00_09.cpp)