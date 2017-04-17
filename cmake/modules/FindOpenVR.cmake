# - Find OpenVR 
#
#  OPENVR_FOUND        - True if OpenVR has been found.
#  OPENVR_INCLUDE_DIRS - OpenVR Include Directory
#  OPENVR_LIBRARIES    - List of libraries when using OpenVR.

set(OPENVR_ROOT_DIR "${OPENVR_ROOT_DIR}" CACHE PATH "Root directory for OpenVR")

# find include dir
find_path(OPENVR_INCLUDE_DIRS NAMES openvr.h HINTS ${OPENVR_ROOT_DIR}/headers)

# determine platform
if(WIN32)
	if(CMAKE_SIZEOF_VOID_P MATCHES "8")
		set(_OPENVR_PLATFORM "win64")
	else()
		set(_OPENVR_PLATFORM "win32")
	endif()
else()
	set(_OPENVR_PLATFORM "linux64")
endif()

# find library
find_library(OPENVR_LIBRARIES NAMES openvr_api HINTS ${OPENVR_ROOT_DIR}/lib/${_OPENVR_PLATFORM})
