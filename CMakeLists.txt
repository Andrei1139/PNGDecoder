cmake_minimum_required(VERSION 3.10.0)
project(ImageToASCII VERSION 0.1.0 LANGUAGES C)

file(GLOB KINFLATE "src/kinflate/*.c")

add_executable(ImageToASCII ${KINFLATE} src/main.c src/data.c src/defilter.c)

