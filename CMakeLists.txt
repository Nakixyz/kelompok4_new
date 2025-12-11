cmake_minimum_required(VERSION 4.0)
project(Review2_1 C)

set(CMAKE_C_STANDARD 23)

add_executable(Review2_1 main.c
        karyawan.h
        interface/crud_akun.h
        interface/crud_karyawan.h
        interface/dashboard_roles.h
        stasiun.h
        interface/crud_operasional.h

)
