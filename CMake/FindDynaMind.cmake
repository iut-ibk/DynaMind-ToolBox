IF(NOT EXISTS ${dynamind_SOURCE_DIR})
    MESSAGE(STATUS "DYNAMIND_DIR $ENV{DYNAMIND_DIR}")
    FIND_PATH(DYNAMIND_INCLUDE_DIR dmmodule.h
                PATHS   ENV DYNAMIND_INCLUDE
                        /usr/include/dynamindcore
                        /usr/local/include/dynamindcore)

    FIND_LIBRARY(DYNAMINDTOOLBOX_LIBRARY NAMES dynamindtoolbox
                PATHS   ENV DYNAMIND_LIB
                        /usr/lib
                        /usr/local/lib)

    FIND_LIBRARY(DYNAMINDCORE_LIBRARY NAMES dynamindcore
                PATHS   ENV DYNAMIND_LIB
                        /usr/local/lib
                        /usr/lib)

    FIND_PATH(DYNAMINDPYTHON pydynamind.py
                PATHS   ENV DYNAMIND_PYTHON

                        /usr/lib
                        /usr/local/lib)

    IF( DYNAMINDPYTHON )
        IF(UNIX)
            SET(DYNAMIND_PYTHON_LIBRARY ${DYNAMINDPYTHON}/pydynamind.py
                                        ${DYNAMINDPYTHON}/_pydynamind.so
                                        ${DYNAMINDPYTHON}/pydmtoolbox.py
                                        ${DYNAMINDPYTHON}/_pydmtoolbox.so)
        ELSE()
            SET(DYNAMIND_PYTHON_LIBRARY ${DYNAMINDPYTHON}\\pydynamind.py
                                        ${DYNAMINDPYTHON}\\_pydynamind.pyd
                                        ${DYNAMINDPYTHON}\\pydmtoolbox.py
                                        ${DYNAMINDPYTHON}\\_pydmtoolbox.pyd)
        ENDIF()
        MESSAGE(STATUS "SET VAR \"DYNAMIND_PYTHON_LIBRARY\" DONE")
    ELSE()
        MESSAGE(STATUS "No DynaMind python installation found")
    ENDIF()

    #FIND DLL FOR WINDOWS INSTALLER
    IF( WIN32 AND NOT UNIX)
        FIND_LIBRARY(3RDPARTYLIB NAMES QtCore4
                    PATHS   ENV DYNAMIND_LIB
                    NO_DEFAULT_PATH)
        IF(${3RDPARTYLIB})
            FILE(GLOB DYNAMIND_INSTALL_LIBRARIES    $ENV{DYNAMIND_LIB}\\QtCore4.dll
                                                    $ENV{DYNAMIND_LIB}\\QtGui4.dll
                                                    $ENV{DYNAMIND_LIB}\\QtXml4.dll
                                                    $ENV{DYNAMIND_LIB}\\QtXmlPatterns4.dll
                                                    $ENV{DYNAMIND_LIB}\\QtNetwork4.dll
                                                    $ENV{DYNAMIND_LIB}\\QtWebKit4.dll
                                                    $ENV{DYNAMIND_LIB}\\phonon4.dll
                                                    $ENV{DYNAMIND_LIB}\\QtOpenGL4.dll
                                                    $ENV{DYNAMIND_LIB}\\QtSVG4.dll
                                                    $ENV{DYNAMIND_LIB}\\QtSql4.dll
                                                    $ENV{DYNAMIND_LIB}\\boost_python-*-mt-1_*.dll
                                                    $ENV{DYNAMIND_LIB}\\boost_system-*-mt-1_*.dll
                                                    $ENV{DYNAMIND_LIB}\\boost_signals-*-mt-1_*.dll
                                                    $ENV{DYNAMIND_LIB}\\boost_date_time-*-mt-1_*.dll
                                                    $ENV{DYNAMIND_LIB}\\boost_filesystem-*-mt-1_*.dll
                                                    $ENV{DYNAMIND_LIB}\\boost_program_options-*-mt-1_*.dll)
            SET(DYNAMIND_INSTALL_MYSQL $ENV{DYNAMIND_LIB}/plugins/sqldrivers/qsqlmysql4.dll)
        ELSE()
            FIND_PACKAGE(Boost 1.38.0 COMPONENTS program_options date_time REQUIRED)
            FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui REQUIRED)

            FILE(GLOB DYNAMIND_INSTALL_LIBRARIES    ${QT_BINARY_DIR}/QtCore4.dll
                                                    ${QT_BINARY_DIR}/QtGui4.dll
                                                    ${QT_BINARY_DIR}/QtXml4.dll
                                                    ${QT_BINARY_DIR}/QtXmlPatterns4.dll
                                                    ${QT_BINARY_DIR}/QtNetwork4.dll
                                                    ${QT_BINARY_DIR}/QtWebKit4.dll
                                                    ${QT_BINARY_DIR}/phonon4.dll
                                                    ${QT_BINARY_DIR}/QtOpenGL4.dll
                                                    ${QT_BINARY_DIR}/QtSVG4.dll
                                                    ${QT_BINARY_DIR}/QtSql4.dll
                                                    ${Boost_LIBRARY_DIRS}/boost_python-*-mt-1_*.dll
                                                    ${Boost_LIBRARY_DIRS}/boost_system-*-mt-1_*.dll
                                                    ${Boost_LIBRARY_DIRS}/boost_signals-*-mt-1_*.dll
                                                    ${Boost_LIBRARY_DIRS}/boost_date_time-*-mt-1_*.dll
                                                    ${Boost_LIBRARY_DIRS}/boost_filesystem-*-mt-1_*.dll
                                                    ${Boost_LIBRARY_DIRS}/boost_program_options-*-mt-1_*.dll)

            SET(DYNAMIND_INSTALL_MYSQL ${QT_BINARY_DIR}/../plugins/sqldrivers/qsqlmysql4.dll)
        ENDIF()

        MESSAGE(STATUS "SET VAR \"DYNAMIND_INSTALL_MYSQL\" DONE")
        MESSAGE(STATUS "SET VAR \"DYNAMIND_INSTALL_LIBRARIES\" DONE")
    ENDIF()


    IF( DYNAMIND_INCLUDE_DIR )
        MESSAGE(FATAL_ERROR "Could not find Dynamind include dir")
    ENDIF()

    IF( DYNAMINDTOOLBOX_LIBRARY )
        MESSAGE(FATAL_ERROR "Could not find Dynamind toolbox library")
    ENDIF()

    IF( DYNAMINDCORE_LIBRARY )
        MESSAGE(FATAL_ERROR "Could not find Dynamind core library")
    ENDIF()

    SET(DYNAMIND_LIBRARY ${DYNAMINDCORE_LIBRARY} ${DYNAMINDTOOLBOX_LIBRARY})
    SET(DYNAMIND_FOUND TRUE)

    IF(DYNAMIND_FOUND)
        MESSAGE(STATUS "SET VAR \"DYNAMIND_INCLUDE_DIR\" DONE")
        MESSAGE(STATUS "SET VAR \"DYNAMIND_LIBRARY\" DONE")
        MESSAGE(STATUS "SET VAR \"DYNAMIND\"")
        MESSAGE(STATUS "DYNAMIND FOUND")
    ELSE()
        MESSAGE(FATAL_ERROR "Could not find Dynamind libraries")
    ENDIF()


    FUNCTION(DYNAMIND_INSTALL_LIBS DIRNAME)
        IF(WIN32)
              INSTALL(FILES ${DYNAMIND_INSTALL_LIBRARIES} DESTINATION ${DIRNAME})
              INSTALL(FILES ${DYNAMIND_INSTALL_MYSQL} DESTINATION ${DIRNAME}/plugins/sqldrivers)
        ENDIF()

        INSTALL(FILES ${DYNAMIND_LIBRARY} DESTINATION ${DIRNAME})
    ENDFUNCTION(DYNAMIND_INSTALL_LIBS)

    MESSAGE(STATUS "\"DYNAMIND_INSTALL_LIBS\" FUNCTION ENABLED")
ELSE()
    IF(WIN32 AND NOT LINUX)
        FUNCTION(DYNAMIND_3RDPARTY_INSTALL_LIBS DIRNAME)
            IF(CMAKE_BUILD_TYPE STREQUAL Release)
                FILE(GLOB DLLS  ${QT_BINARY_DIR}/QtCore4.dll
                                ${QT_BINARY_DIR}/QtGui4.dll
                                ${QT_BINARY_DIR}/QtXml4.dll
                                ${QT_BINARY_DIR}/QtXmlPatterns4.dll
                                ${QT_BINARY_DIR}/QtNetwork4.dll
                                ${QT_BINARY_DIR}/QtWebKit4.dll
                                ${QT_BINARY_DIR}/phonon4.dll
                                ${QT_BINARY_DIR}/QtOpenGL4.dll
                                ${QT_BINARY_DIR}/QtSVG4.dll
                                ${QT_BINARY_DIR}/QtSql4.dll
                                ${Boost_LIBRARY_DIRS}/boost_python-*-mt-1_*.dll
                                ${Boost_LIBRARY_DIRS}/boost_system-*-mt-1_*.dll
                                ${Boost_LIBRARY_DIRS}/boost_signals-*-mt-1_*.dll
                                ${Boost_LIBRARY_DIRS}/boost_date_time-*-mt-1_*.dll
                                ${Boost_LIBRARY_DIRS}/boost_filesystem-*-mt-1_*.dll
                                ${Boost_LIBRARY_DIRS}/boost_program_options-*-mt-1_*.dll)
            ENDIF()

            INSTALL(FILES ${DLLS} DESTINATION bin)
            INSTALL(FILES ${QT_BINARY_DIR}/../plugins/sqldrivers/qsqlmysql4.dll DESTINATION ${DIRNAME}/plugins/sqldrivers)
        ENDFUNCTION(DYNAMIND_3RDPARTY_INSTALL_LIBS)

        MESSAGE(STATUS "\"DYNAMIND_3RDPARTY_INSTALL_LIBS\" FUNCTION ENABLED")
    ENDIF()
ENDIF()
