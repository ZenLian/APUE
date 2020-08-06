cmake_minimum_required(VERSION 2.8.2)
project(apue)

include_directories(${library_INCLUDE_DIRS})

add_executable(ls ls.c)
