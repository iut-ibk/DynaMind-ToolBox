cmake_minimum_required(VERSION 2.6)
project(sqlite3_dm)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")

file(GLOB SRCS *.c *.h)

	ADD_DEFINITIONS(-DSOL)
    message(STATUS "Compiling as library")

    IF(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
        ADD_DEFINITIONS(/D _CRT_SECURE_NO_WARNINGS)
        ADD_DEFINITIONS(/D DLL)
    ELSE()
        ADD_DEFINITIONS(-Wno-format-security -Wno-format -fPIC)
        ADD_DEFINITIONS(-DSOL)
    ENDIF()

	ADD_DEFINITIONS(-DSQLITE_ENABLE_RTREE=1)

    message(STATUS "Compiling as library")
	add_library(sqlite3_dm STATIC ${SRCS})


target_link_libraries(sqlite3_dm)
