cmake_minimum_required(VERSION 4.0)
project(Review2_1 C)

set(CMAKE_C_STANDARD 23)

add_executable(Review2_1 main.c
        karyawan.h
        stasiun/createStasiun.h
        stasiun/readStasiun.h
        stasiun/updateStasiun.h
        stasiun/deleteStasiun.h

)
