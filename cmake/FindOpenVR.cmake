# find openvr
# finds openvr lib
#
# defines:
#   OpenVR_FOUND        - found openvr
#   OpenVR_INCLUDE_DIRS - include dir
#   OpenVR_LIBRARIES    - lib files
#   OpenVR_VERSION      - version

include(FindPackageHandleStandardArgs)

# find headers
find_path(OpenVR_INCLUDE_DIR
    NAMES openvr.h
    PATHS
        ${CMAKE_CURRENT_SOURCE_DIR}/external/openvr/headers
        ${CMAKE_CURRENT_SOURCE_DIR}/../openvr/headers
        ${CMAKE_CURRENT_SOURCE_DIR}/../../openvr/headers
        $ENV{OPENVR_SDK}/headers
        $ENV{STEAMVR_SDK}/headers
        /usr/include/openvr
        /usr/local/include/openvr
)

# find lib
if(WIN32)
    find_library(OpenVR_LIBRARY
        NAMES openvr_api
        PATHS
            ${CMAKE_CURRENT_SOURCE_DIR}/external/openvr/lib/win64
            ${CMAKE_CURRENT_SOURCE_DIR}/../openvr/lib/win64
            ${CMAKE_CURRENT_SOURCE_DIR}/../../openvr/lib/win64
            $ENV{OPENVR_SDK}/lib/win64
            $ENV{STEAMVR_SDK}/lib/win64
    )
elseif(UNIX)
    find_library(OpenVR_LIBRARY
        NAMES openvr_api
        PATHS
            ${CMAKE_CURRENT_SOURCE_DIR}/external/openvr/lib/linux64
            ${CMAKE_CURRENT_SOURCE_DIR}/../openvr/lib/linux64
            ${CMAKE_CURRENT_SOURCE_DIR}/../../openvr/lib/linux64
            $ENV{OPENVR_SDK}/lib/linux64
            $ENV{STEAMVR_SDK}/lib/linux64
            /usr/lib
            /usr/local/lib
    )
endif()

# get version
if(EXISTS "${OpenVR_INCLUDE_DIR}/openvr.h")
    file(STRINGS "${OpenVR_INCLUDE_DIR}/openvr.h" OpenVR_VERSION_LINE
        REGEX "^#define[ \t]+k_nSteamVRVersionMajor[ \t]+[0-9]+$")
    string(REGEX REPLACE "^#define[ \t]+k_nSteamVRVersionMajor[ \t]+([0-9]+)$" "\\1"
        OpenVR_VERSION_MAJOR "${OpenVR_VERSION_LINE}")
    file(STRINGS "${OpenVR_INCLUDE_DIR}/openvr.h" OpenVR_VERSION_LINE
        REGEX "^#define[ \t]+k_nSteamVRVersionMinor[ \t]+[0-9]+$")
    string(REGEX REPLACE "^#define[ \t]+k_nSteamVRVersionMinor[ \t]+([0-9]+)$" "\\1"
        OpenVR_VERSION_MINOR "${OpenVR_VERSION_LINE}")
    set(OpenVR_VERSION "${OpenVR_VERSION_MAJOR}.${OpenVR_VERSION_MINOR}")
endif()

set(OpenVR_INCLUDE_DIRS ${OpenVR_INCLUDE_DIR})
set(OpenVR_LIBRARIES ${OpenVR_LIBRARY})

find_package_handle_standard_args(OpenVR
    REQUIRED_VARS OpenVR_LIBRARY OpenVR_INCLUDE_DIR
    VERSION_VAR OpenVR_VERSION
)

mark_as_advanced(OpenVR_INCLUDE_DIR OpenVR_LIBRARY) 