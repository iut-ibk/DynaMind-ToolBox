FILE(COPY ${dynamind_SOURCE_DIR}/scripts DESTINATION ${SCRIPT_PATH})


INSTALL(DIRECTORY ${dynamind_SOURCE_DIR}/scripts/
		DESTINATION "share/DynaMind/python_extensions")

