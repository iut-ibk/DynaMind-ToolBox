

FILE(COPY ${dynamind-performance-assessment_SOURCE_DIR}/3rdparty/CD3Waterbalance DESTINATION ${SCRIPT_PATH}/../CD3Modules)

INSTALL(DIRECTORY ${dynamind-performance-assessment_BINARY_DIR}/scripts/
		DESTINATION "share/DynaMind/CD3Modules")

