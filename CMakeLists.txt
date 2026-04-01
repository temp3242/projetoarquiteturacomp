cmake_minimum_required(VERSION 4.1)
project(projetoarquitetura)

set(CMAKE_CXX_STANDARD 20)

configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.in"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/config.h"
        @ONLY
)

add_executable(projetoarquitetura src/main.cpp
        src/etapa1/etapa1.cpp
        src/etapa1/etapa1.h
        src/etapa2T1/etapa2.cpp
        src/etapa2T1/etapa2.h
        src/etapa2T2/etapa2t2.cpp
        src/etapa2T2/etapa2t2.h
        src/etapa3T1/etapa3t1.cpp
        src/etapa3T1/etapa3t1.h
        src/etapa3e/etapa3e.cpp
        src/etapa3e/etapa3e.h
        )
