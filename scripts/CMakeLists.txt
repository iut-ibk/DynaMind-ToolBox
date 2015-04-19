FILE(COPY ${dynamind-gdalmodules_SOURCE_DIR}/scripts DESTINATION ${SCRIPT_PATH})

INSTALL(DIRECTORY ${dynamind-gdalmodules_SOURCE_DIR}/scripts/
		DESTINATION "share/DynaMind/python_extensions")
