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
#add_executable(Ex02_01 01_boost/example02/Ex02_01.cpp)
#add_executable(Ex02_02 01_boost/example02/Ex02_02.cpp)
#add_executable(Ex02_03 01_boost/example02/Ex02_03.cpp)
#add_executable(Ex02_04 01_boost/example02/Ex02_04.cpp)
#add_executable(Ex02_05 01_boost/example02/Ex02_05.cpp)
#add_executable(Ex02_06 01_boost/example02/Ex02_06.cpp)
#add_executable(Ex02_07 01_boost/example02/Ex02_07.cpp)
#add_executable(Ex02_08 01_boost/example02/Ex02_08.cpp)
#add_executable(Ex02_09 01_boost/example02/Ex02_09.cpp)
#add_executable(Ex02_10 01_boost/example02/Ex02_10.cpp)
##########example03##########
#add_executable(Ex03_01 01_boost/example03/Ex03_01.cpp)
#add_executable(Ex03_02 01_boost/example03/Ex03_02.cpp)
#add_executable(Ex03_03 01_boost/example03/Ex03_03.cpp)
#add_executable(Ex03_04 01_boost/example03/Ex03_04.cpp)
#add_executable(Ex03_05 01_boost/example03/Ex03_05.cpp)
#add_executable(Ex03_06 01_boost/example03/Ex03_06.cpp)
#add_executable(Ex03_07 01_boost/example03/Ex03_07.cpp)
#add_executable(Ex03_08 01_boost/example03/Ex03_08.cpp)
#add_executable(Ex03_09 01_boost/example03/Ex03_09.cpp)
#add_executable(Ex03_10 01_boost/example03/Ex03_10.cpp)
#add_executable(Ex03_11 01_boost/example03/Ex03_11.cpp)
#add_executable(Ex03_12 01_boost/example03/Ex03_12.cpp)
#add_executable(Ex03_13 01_boost/example03/Ex03_13.cpp)
#add_executable(Ex03_14 01_boost/example03/Ex03_14.cpp)
#add_executable(Ex03_15 01_boost/example03/Ex03_15.cpp)
#add_executable(Ex03_16 01_boost/example03/Ex03_16.cpp)
#add_executable(Ex03_17 01_boost/example03/Ex03_17.cpp)
##add_executable(Ex03_18 01_boost/example03/Ex03_18.cpp)
#add_executable(Ex03_19 01_boost/example03/Ex03_19.cpp)
#add_executable(Ex03_20 01_boost/example03/Ex03_20.cpp)
#add_executable(Ex03_21 01_boost/example03/Ex03_21.cpp)
#add_executable(Ex03_22 01_boost/example03/Ex03_22.cpp)
#add_executable(Ex03_23 01_boost/example03/Ex03_23.cpp)
##add_executable(Ex03_24 01_boost/example03/Ex03_24.cpp)
#add_executable(Ex03_25 01_boost/example03/Ex03_25.cpp)
#add_executable(Ex03_26 01_boost/example03/Ex03_26.cpp)
#add_executable(Ex03_27 01_boost/example03/Ex03_27.cpp)
#add_executable(Ex03_28 01_boost/example03/Ex03_28.cpp)
#add_executable(Ex03_29 01_boost/example03/Ex03_29.cpp)
#add_executable(Ex03_30 01_boost/example03/Ex03_30.cpp)
#add_executable(Ex03_31 01_boost/example03/Ex03_31.cpp)
#add_executable(Ex03_32 01_boost/example03/Ex03_32.cpp)
##########example04##########
add_executable(Ex04_01 01_boost/example04/Ex04_01.cpp)
add_executable(Ex04_02 01_boost/example04/Ex04_02.cpp)
add_executable(Ex04_03 01_boost/example04/Ex04_03.cpp)
add_executable(Ex04_04 01_boost/example04/Ex04_04.cpp)
add_executable(Ex04_05 01_boost/example04/Ex04_05.cpp)
add_executable(Ex04_06 01_boost/example04/Ex04_06.cpp)
add_executable(Ex04_07 01_boost/example04/Ex04_07.cpp)
add_executable(Ex04_08 01_boost/example04/Ex04_08.cpp)
add_executable(Ex04_09 01_boost/example04/Ex04_09.cpp)
add_executable(Ex04_10 01_boost/example04/Ex04_10.cpp)
add_executable(Ex04_11 01_boost/example04/Ex04_11.cpp)
add_executable(Ex04_12 01_boost/example04/Ex04_12.cpp)
add_executable(Ex04_13 01_boost/example04/Ex04_13.cpp)
add_executable(Ex04_14 01_boost/example04/Ex04_14.cpp)
add_executable(Ex04_15 01_boost/example04/Ex04_15.cpp)
add_executable(Ex04_16 01_boost/example04/Ex04_16.cpp)
add_executable(Ex04_17 01_boost/example04/Ex04_17.cpp)
add_executable(Ex04_18 01_boost/example04/Ex04_18.cpp)
add_executable(Ex04_19 01_boost/example04/Ex04_19.cpp)
add_executable(Ex04_20 01_boost/example04/Ex04_20.cpp)
add_executable(Ex04_21 01_boost/example04/Ex04_21.cpp)
add_executable(Ex04_22 01_boost/example04/Ex04_22.cpp)
add_executable(Ex04_23 01_boost/example04/Ex04_23.cpp)
add_executable(Ex04_24 01_boost/example04/Ex04_24.cpp)
add_executable(Ex04_25 01_boost/example04/Ex04_25.cpp)
add_executable(Ex04_26 01_boost/example04/Ex04_26.cpp)
##########example05##########

##########example06##########

##########example07##########

##########example08##########

##########example09##########

##########example10##########