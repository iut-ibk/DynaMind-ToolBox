cmake_minimum_required(VERSION 2.8)
project(DynaMind-ToolBox)


find_package(Git REQUIRED)

set(TOOLBOX DynaMind DynaMind-Gui DynaMind-BasicModules DynaMind-Sewer DynaMind-CityBlocks DynaMind-QGIS)


foreach (tool ${TOOLBOX})
	if (EXISTS ${CMAKE_SOURCE_DIR}/${tool}/src)
	    message(STATUS "${tool} submodule already exists")
	else (EXISTS ${CMAKE_SOURCE_DIR}/${tool}/src)
	    message(STATUS "preparing ${tool} submodule")
	    execute_process(COMMAND ${GIT_EXECUTABLE} submodule init
		            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
	    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update
		            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
	endif (EXISTS ${CMAKE_SOURCE_DIR}/${tool}/src)


	
endforeach(${tool}) 


SET(DYNAMIND_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/DynaMind/src/core) 
SET(DYNAMIND_INCLUDE_DIR ${DYNAMIND_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/DynaMind/src/DMToolboxes) 
SET(DYNAMIND_INCLUDE_DIR ${DYNAMIND_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/DynaMind/src/python-swig) 

foreach (tool ${TOOLBOX})
	add_subdirectory(${tool})
endforeach(${tool}) 
