FILE(COPY ${dynamind-performance-assessment_SOURCE_DIR}/scripts DESTINATION ${SCRIPT_PATH})

INSTALL(DIRECTORY ${dynamind-performance-assessment_SOURCE_DIR}/scripts/
		DESTINATION "share/DynaMind/PythonModules")


#MACRO(list_subdirectories retval curdir return_relative)
#  FILE(GLOB sub-dir RELATIVE ${curdir} *)
#  SET(list_of_dirs "")
#  FOREACH(dir ${sub-dir})
#    IF(IS_DIRECTORY ${curdir}/${dir})
#      IF(${return_relative})
#        SET(list_of_dirs ${list_of_dirs} ${dir})
#      ELSE()
#        SET(list_of_dirs ${list_of_dirs} ${curdir}/${dir})
#      ENDIF()
#    ENDIF()
#  ENDFOREACH()
#  SET(${retval} ${list_of_dirs})
#  MESSAGE(STATUS "Found subdirectories in ${curdir}: ${list_of_dirs}")
#ENDMACRO()

#LIST_SUBDIRECTORIES(DIRS ${dynamind-performance-assessment_SOURCE_DIR}/scripts/ 1)



#FOREACH(DIR ${DIRS})
#    FILE(GLOB SCRIPTFILES "${dynamind-performance-assessment_SOURCE_DIR}/scripts/${DIR}/*.py")
#    INSTALL(FILES ${SCRIPTFILES} DESTINATION "${SCRIPT_PATH}/${DIR}")
#ENDFOREACH(DIR)

#FILE(GLOB BASESCRIPTS "${dynamind-performance-assessment_SOURCE_DIR}/scripts/*.py")
#INSTALL(FILES ${BASESCRIPTS} DESTINATION "${SCRIPT_PATH}")
