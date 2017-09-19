cmake_minimum_required(VERSION 2.6)
project(swmm5)

option(SWMM_LIB "compile as library" ON)

FIND_PACKAGE(OpenMP)

IF(OPENMP_FOUND)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
ENDIF()

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
	add_library(swmm5 SHARED ${SRCS})
else (SWMM_LIB)
    message(STATUS "Compiling as standalone CLI application")
    add_definitions(-DCLE)
    add_executable(swmm5 ${SRCS})
endif(SWMM_LIB)

target_link_libraries(swmm5 -lm)

INSTALL(TARGETS swmm5
		DESTINATION "share/DynaMind/Modules"
		PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
