#
# - Find vsqlite
# Find the native VSQLITE includes and library
#
#  VSQLITE_INCLUDE_DIRS - where to find sqlite/connection.hpp, etc.
#  VSQLITE_LIBRARIES    - List of libraries when using pcre.
#  VSQLITE_FOUND        - True if pcre found.


IF (VSQLITE_INCLUDE_DIRS)
  # Already in cache, be silent
  SET(VSQLITE_FIND_QUIETLY TRUE)
ENDIF (VSQLITE_INCLUDE_DIRS)

FIND_PATH(VSQLITE_INCLUDE_DIR sqlite/connection.hpp)

SET(VSQLITE_NAMES vsqlitepp)
# IF (VSQLITE_LIBRARIES)
#   FIND_LIBRARY(VSQLITE_LIBRARY NAMES ${VSQLITE_NAMES} HINTS ${VSQLITE_LIBRARIES})
# ELSE()
#   FIND_LIBRARY(VSQLITE_LIBRARY NAMES ${VSQLITE_NAMES} )
# ENDIF()

if(VSQLITE_LIBRARIES)
  # Converto to a list of library argments
  string(REPLACE " " ";" VSQLITE_LIB_ARGS ${VSQLITE_LIBRARIES})

  # Parse the list in order to find the library path
  foreach(VSQLITE_LIB_ARG ${VSQLITE_LIB_ARGS})
    string(REPLACE "-L" "" VSQLITE_LIB_ARG_CLEAR ${VSQLITE_LIB_ARG})
    if(NOT ${VSQLITE_LIB_ARG_CLEAR} STREQUAL ${VSQLITE_LIB_ARG})
      set(VSQLITE_SUPPLIED_LIB_DIR ${VSQLITE_LIB_ARG_CLEAR})
    endif()
  endforeach(VSQLITE_LIB_ARG)
  find_library(VSQLITE_LIBRARY NAMES ${VSQLITE_NAMES} HINTS ${VSQLITE_SUPPLIED_LIB_DIR})
  
  unset(VSQLITE_LIB_ARG_CLEAR)
  unset(VSQLITE_LIB_ARG)
  unset(VSQLITE_LIB_ARGS)
else()
  find_library(VSQLITE_LIBRARY NAMES ${VSQLITE_NAMES})
endif()

get_filename_component(VSQLITE_LIB_FILENAME ${VSQLITE_LIBRARY} NAME_WE)
# message("VSQLITE_LIB_FILENAME:" ${VSQLITE_LIB_FILENAME})
get_filename_component(VSQLITE_LIB_DIRECTORY ${VSQLITE_LIBRARY} PATH)
# message("VSQLITE_LIB_DIRECTORY:" ${VSQLITE_LIB_DIRECTORY})
get_filename_component(VSQLITE_LIB_BASE_DIRECTORY ${VSQLITE_LIB_DIRECTORY} PATH)
# message("VSQLITE_LIB_BASE_DIRECTORY:" ${VSQLITE_LIB_BASE_DIRECTORY})

# handle the QUIETLY and REQUIRED arguments and set VSQLITE_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(VSqlite DEFAULT_MSG VSQLITE_LIBRARY VSQLITE_INCLUDE_DIR)

IF(VSQLITE_FOUND)
  SET( VSQLITE_LIBRARIES "-L${VSQLITE_LIB_DIRECTORY} -l${VSQLITE_NAMES}" )
  SET( VSQLITE_INCLUDE_DIRS ${VSQLITE_INCLUDE_DIR} )
ELSE(VSQLITE_FOUND)
  SET( VSQLITE_LIBRARIES )
  SET( VSQLITE_INCLUDE_DIRS )
ENDIF(VSQLITE_FOUND)

message("VSQLITE INCLUDE DIR: ${VSQLITE_INCLUDE_DIRS}")
message("VSQLITE LIB DIR: ${VSQLITE_LIBRARIES}")

MARK_AS_ADVANCED( VSQLITE_LIBRARIES VSQLITE_INCLUDE_DIRS )