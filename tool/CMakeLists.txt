cmake_minimum_required(VERSION 3.16)

project(fw_update C)

include_directories(. G5Algo G4Algo ../tool ../hw)

file(GLOB SOURCES "*.c" "G5Algo/*.c" "../hw/*.c")

add_executable(fw_update ${SOURCES})

install(TARGETS fw_update)
