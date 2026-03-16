cmake_minimum_required(VERSION 4.1)
project(projetoarquitetura)

set(CMAKE_CXX_STANDARD 20)

add_executable(projetoarquitetura src/main.cpp
        src/etapa1/etapa1.cpp
        src/etapa1/etapa1.h
        src/etapa2T1/etapa2.cpp
        src/etapa2T1/etapa2.h
        src/etapa2T2/etapa2t2.cpp
        src/etapa2T2/etapa2t2.h)
