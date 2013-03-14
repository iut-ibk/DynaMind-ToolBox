file(GLOB EXE_CPPS *.cpp)

add_executable(dynamind ${EXE_CPPS})
target_link_libraries(dynamind ${DYNAMIND_CORE_LIBRARY} ${QT_QTMAIN_LIBRARY} ${Boost_LIBRARIES} ${QT_LIBRARIES} ${PYTHON_LIBRARIES})


FIND_PATH(DYNAMINDDEV DynaMindDevCmake.txt
            PATHS   $ENV{DYNAMIND_DIR}/include
            PATH_SUFFIXES dynamindcore/CMake)


IF(DYNAMIND_INCLUDE_DIR)
        MESSAGE(STATUS "Compiled from toolbox")
        MESSAGE(STATUS "DYNAMIND_INCLUDE_DIR ${DYNAMIND_INCLUDE_DIR}")
        SET (DYNAMIND_CORE_LIBRARY dynamindcore)
        SET (DYNAMIND_TOOLBOX_LIBRARY dynamindtoolbox)
        SET (DYNAMIND_EXTENSIONS_LIBRARY dynamind-extensions)
        MESSAGE(STATUS "${DYNAMIND_CORE_LIBRARY}")
ELSE()
        MESSAGE(STATUS "Compiled from stand alone")
        FIND_PATH(DYNAMINDDEV DynaMindDevCmake.txt
        PATHS $ENV{DYNAMIND_DIR}/include
        PATH_SUFFIXES dynamindcore/CMake)

	IF(DYNAMINDDEV)
		INCLUDE(${DYNAMINDDEV}/DynaMindDevCmake.txt)
	ELSE()
		MESSAGE(FATAL_ERROR "Could not find dynamind. Maybe not installed ?")
	ENDIF()
	ELSE()
		MESSAGE(STATUS "Compiled from toolbox")
		MESSAGE(STATUS "DYNAMIND_INCLUDE_DIR ${DYNAMIND_INCLUDE_DIR}")
		SET (DYNAMIND_CORE_LIBRARY dynamindcore)
		MESSAGE(STATUS "${DYNAMIND_CORE_LIBRARY}")
ENDIF()

# Installation of the program
INSTALL(TARGETS dynamind
        DESTINATION "bin"
        PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)