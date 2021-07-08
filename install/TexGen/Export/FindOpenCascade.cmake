#
# Find the OpenCascade includes and library
#
# This module defines
# OPENCASCADE_INCLUDE_DIR, where to find headers
# OPENCASCADE_LIBRARY_DIR, where to find the OpenCascade library.
# OPENCASCADE_DEBUG_LIBRARY_DIR, where to find the OpenCascade library in debug mode.
# OPENCASCADE_FOUND, If false, do not try to use OpenCascade.

FIND_PATH(OPENCASCADE_INCLUDE_DIR gp.hxx
  /usr/local/include
  /usr/include
)

# With Win32, important to have both
IF(WIN32)
  FIND_PATH(OPENCASCADE_LIBRARY_DIR TKGeomBase.lib
               ${CPPUNIT_INCLUDE_DIR}/../win32/lib)
  FIND_PATH(OPENCASCADE_DEBUG_LIBRARY_DIR TKGeomBase
               ${CPPUNIT_INCLUDE_DIR}/../win32/libd)
ELSE(WIN32)
  # On unix system, debug and release have the same name
  FIND_PATH(OPENCASCADE_LIBRARY_DIR TKGeomBase.lib
               /usr/local/lib
               /usr/lib)
  FIND_PATH(OPENCASCADE_DEBUG_LIBRARY_DIR TKGeomBase
               /usr/local/lib
               /usr/lib)
ENDIF(WIN32)

IF(OPENCASCADE_INCLUDE_DIR)
  IF(OPENCASCADE_LIBRARY_DIR)
    SET(OPENCASCADE_FOUND "YES")
  ENDIF(OPENCASCADE_LIBRARY_DIR)
ENDIF(OPENCASCADE_INCLUDE_DIR)
