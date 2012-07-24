cmake_minimum_required(VERSION 2.8)
project(DynaMind-ToolBox)


find_package(Git REQUIRED)

set(TOOLBOX DynaMind DynaMind-Gui DynaMind-BasicModules DynaMind-Sewer DynaMind-CityBlocks)
file(MAKE_DIRECTORY ${DynaMind-ToolBox_BINARY_DIR}/output)


foreach (tool ${TOOLBOX})
	if (EXISTS ${CMAKE_SOURCE_DIR}/${tool}/src)
	    message(STATUS "${tool} submodule already exists")

	else (EXISTS ${CMAKE_SOURCE_DIR}/${tool}/src)
	    message(STATUS "preparing ${tool} submodule")
	    execute_process(COMMAND ${GIT_EXECUTABLE} submodule init ${tool}
		            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
	    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update ${tool}
		            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
	    execute_process(COMMAND ${GIT_EXECUTABLE} checkout master
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/${tool}/)
        endif (EXISTS ${CMAKE_SOURCE_DIR}/${tool}/src)
        execute_process(COMMAND ${GIT_EXECUTABLE} checkout master
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/${tool}/)
        execute_process(COMMAND ${GIT_EXECUTABLE} pull origin master WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/${tool}/)

endforeach(${tool}) 

SET(DYNAMIND_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/DynaMind/src/core) 
SET(DYNAMIND_INCLUDE_DIR ${DYNAMIND_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/DynaMind/src/DMToolboxes) 
SET(DYNAMIND_INCLUDE_DIR ${DYNAMIND_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/DynaMind/src/python-swig) 
SET(DYNAMIND_TOOLBOX_BUILD_DIR ${DynaMind-ToolBox_BINARY_DIR}/output)

foreach (tool ${TOOLBOX})
	add_subdirectory(${tool})
endforeach(${tool}) 


add_subdirectory(DynaMind-ComplexUnitTests)
add_subdirectory(Data)
INCLUDE(DynaMind-ToolBoxCPack.txt)
INCLUDE(DynaMind-ToolBoxCPack_with_Python.txt)
