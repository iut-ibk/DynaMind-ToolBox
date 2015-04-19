FILE(COPY ${dynamind-basicmodules_SOURCE_DIR}/scripts DESTINATION ${SCRIPT_PATH})

INSTALL(DIRECTORY ${dynamind-basicmodules_SOURCE_DIR}/scripts/
		DESTINATION "share/DynaMind/python_extensions")
