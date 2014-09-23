cmake_minimum_required(VERSION 2.6)
project(swmm5)

option(SWMM_LIB "compile as library" ON)

find_package(OpenMP)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")

file(GLOB SRCS *.c *.h)

if (SWMM_LIB)
    message(STATUS "Compiling as library")

    IF(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
        ADD_DEFINITIONS(/D _CRT_SECURE_NO_WARNINGS)
        ADD_DEFINITIONS(/D DLL)
    ELSE()
        ADD_DEFINITIONS(-Wno-format-security -Wno-format -fPIC)
        ADD_DEFINITIONS(-DSOL)
    ENDIF()

    message(STATUS "Compiling as library")
    add_library(swmm5 STATIC ${SRCS})
else (SWMM_LIB)
    message(STATUS "Compiling as standalone CLI application")
    add_definitions(-DCLE)
    add_executable(swmm5 ${SRCS})
endif(SWMM_LIB)

target_link_libraries(swmm5 -lm)
