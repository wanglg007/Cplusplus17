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
####################(2)boostexec####################
##########example00##########
#add_executable(Ex00_01 01_boost/example00/Ex00_01.cpp)
#add_executable(Ex00_02 01_boost/example00/Ex00_02.cpp)
#add_executable(Ex00_03 01_boost/example00/Ex00_03.cpp)
#add_executable(Ex00_04 01_boost/example00/Ex00_04.cpp)
#add_executable(Ex00_05 01_boost/example00/Ex00_05.cpp)
##########example01##########
#add_executable(Ex01_01 01_boost/example01/Ex01_01.cpp)
#add_executable(Ex01_02 01_boost/example01/Ex01_02.cpp)
#add_executable(Ex01_03 01_boost/example01/Ex01_03.cpp)
#add_executable(Ex01_04 01_boost/example01/Ex01_04.cpp)
#add_executable(Ex01_05 01_boost/example01/Ex01_05.cpp)
#add_executable(Ex01_06 01_boost/example01/Ex01_06.cpp)
#add_executable(Ex01_07 01_boost/example01/Ex01_07.cpp)
#add_executable(Ex01_08 01_boost/example01/Ex01_08.cpp)
#add_executable(Ex01_09 01_boost/example01/Ex01_09.cpp)
##add_executable(Ex01_10 01_boost/example01/Ex01_10.cpp)
#add_executable(Ex01_11 01_boost/example01/Ex01_11.cpp)
#add_executable(Ex01_12 01_boost/example01/Ex01_12.cpp)
#add_executable(Ex01_13 01_boost/example01/Ex01_13.cpp)
##add_executable(Ex01_14 01_boost/example01/Ex01_14.cpp)
##add_executable(Ex01_15 01_boost/example01/Ex01_15.cpp)
#add_executable(Ex01_16 01_boost/example01/Ex01_16.cpp)
#add_executable(Ex01_17 01_boost/example01/Ex01_17.cpp)
#add_executable(Ex01_18 01_boost/example01/Ex01_18.cpp)
#add_executable(Ex01_19 01_boost/example01/Ex01_19.cpp)
##########example02#########

##########example03##########

##########example04##########