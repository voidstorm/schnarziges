#
#
# Try to find the GLFW libraries
# Once done this will define
#
# GLFW_FOUND          - system has GLFW
# GLFW_INCLUDE_DIR    - path to GLFW/GLFW.h
# GLFW_LIBRARY      - the library that must be included
#
#

include(FindPackageHandleStandardArgs)
include(FindLibraryDebugRelease)

find_path(VEZ_INCLUDE_DIR include/vez.h)


find_library_debug_release(VEZ_LIBRARY "VEZ")

if(VEZ_INCLUDE_DIR)
    set(VEZ_FOUND TRUE)
endif()

find_package_handle_standard_args(VEZ DEFAULT_MSG VEZ_INCLUDE_DIR)

mark_as_advanced(VEZ_INCLUDE_DIR VEZ_LIBRARY)