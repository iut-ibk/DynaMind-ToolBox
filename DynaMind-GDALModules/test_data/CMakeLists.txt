FILE(COPY ${dynamind-gdalmodules_SOURCE_DIR}/test_data/ DESTINATION ${TEST_DATA_PATH})

INSTALL(DIRECTORY ${dynamind-gdalmodules_SOURCE_DIR}/test_data/
		DESTINATION "share/DynaMind/test_data")
