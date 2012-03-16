find_package(PkgConfig)

FIND_PATH(VIBE2CORE_INCLUDE_DIRS module.h  PATH_SUFFIXES vibe2core) 
FIND_LIBRARY(VIBE2CORE_LIBRARIES  vibe2core vibe2helper)  

include(FindPackageHandleStandardArgs)
