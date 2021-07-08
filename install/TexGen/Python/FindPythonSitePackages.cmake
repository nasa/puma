#
# Find the Python site-packages directory
#
# This module defines
# PYTHON_SITEPACKAGES_DIR, where to find the Python site packages
# PYTHON_SITEPACKAGES_FOUND, If false, do not try to use OpenCascade.

FIND_PATH(PYTHON_SITEPACKAGES_DIR README.txt
  /usr/local/lib/python2.5/site-packages
  /usr/lib/python2.5/site-packages
  NO_DEFAULT_PATH
)

IF(PYTHON_SITEPACKAGES_DIR)
  SET(PYTHON_SITEPACKAGES_FOUND "YES")
ENDIF(PYTHON_SITEPACKAGES_DIR)
