file(GLOB VIBELIB_CPPS *.cpp alglib/src/*.cpp)
file(GLOB VIBEHELPER_CPPS *.h)

add_library(vibe2helper SHARED ${VIBELIB_CPPS})
include_directories(alglib/src)

if(WIN32)
        target_link_libraries(vibe2helper vibe2core ${QT_LIBRARIES} ${CGAL_LIBRARIES})
else()
        target_link_libraries(vibe2helper vibe2core ${QT_LIBRARIES} ${CGAL_LIBRARIES})
endif()
if(WIN32)
	GET_TARGET_PROPERTY(LIB_NAME vibe2helper LOCATION)
	SET(NEW_LIB_NAME  ${CMAKE_BINARY_DIR}/vibe2helper.pyd)
	ADD_CUSTOM_COMMAND(
	   TARGET vibe2helper
	   POST_BUILD
	   COMMAND ${CMAKE_COMMAND} -E copy ${LIB_NAME} ${NEW_LIB_NAME})
else()
	set_target_properties(vibe2helper
	PROPERTIES
	OUTPUT_NAME "vibe2helper"
	PREFIX ""
)
endif()

INSTALL(TARGETS vibe2helper
        DESTINATION "lib"
        PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
INSTALL(FILES ${VIBEHELPER_CPPS}
        DESTINATION "include/vibe2core"
        PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)
