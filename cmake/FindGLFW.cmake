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

find_path(GLWF_INCLUDE_DIR GLWF/glfw3.h)


find_library_debug_release(GLWF_LIBRARY "GLWF-vc140-mt")

if(GLWF_INCLUDE_DIR)
    set(GLWF_FOUND TRUE)
endif()

find_package_handle_standard_args(GLWF DEFAULT_MSG GLWF_INCLUDE_DIR)

mark_as_advanced(GLWF_INCLUDE_DIR GLWF_LIBRARY)