cmake_minimum_required(VERSION 3.10)
project(OpenTrackServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add cmake modules path
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Find required packages
find_package(OpenVR REQUIRED)

# Add driver source files
set(DRIVER_SOURCES
    src/driver.cpp
    src/tracker_device_driver.cpp
    src/tracker_api.cpp
    src/tracker_udp_server.cpp
)

# Create the driver library
add_library(${PROJECT_NAME} SHARED ${DRIVER_SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${OpenVR_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/external/openvr/headers
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${OpenVR_LIBRARIES}
)

# Set output directories
set(DIST_DIR "${CMAKE_BINARY_DIR}/dist")
set(DRIVER_DIR "${DIST_DIR}/driver_${PROJECT_NAME}")

# Platform-specific output name and paths
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME "driver_${PROJECT_NAME}"
        PREFIX ""
        SUFFIX ".dll"
        RUNTIME_OUTPUT_DIRECTORY "${DRIVER_DIR}/bin/win64"
        LIBRARY_OUTPUT_DIRECTORY "${DRIVER_DIR}/bin/win64"
    )
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME "lib${PROJECT_NAME}"
        PREFIX ""
        SUFFIX ".so"
        RUNTIME_OUTPUT_DIRECTORY "${DRIVER_DIR}/bin"
        LIBRARY_OUTPUT_DIRECTORY "${DRIVER_DIR}/bin"
    )
endif()

# Create distribution directory structure
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${DRIVER_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/driver.vrdrivermanifest"
        "${DRIVER_DIR}/"
    # Copy the appropriate install script to the root dist folder
    $<$<PLATFORM_ID:Windows>:
        ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/install_windows.bat" "${DIST_DIR}/install_windows.bat">
    $<$<NOT:$<PLATFORM_ID:Windows>>:
        ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/install_linux.sh" "${DIST_DIR}/install_linux.sh">
    COMMENT "Creating distribution directory structure and copying install script to dist root"
)

# Create install rules
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION "driver_${PROJECT_NAME}/bin/win64"
    LIBRARY DESTINATION "driver_${PROJECT_NAME}/bin"
)

install(FILES driver.vrdrivermanifest
    DESTINATION "driver_${PROJECT_NAME}"
)

# Add custom target for creating distribution package
add_custom_target(dist
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${DRIVER_DIR}"
        "${DIST_DIR}/driver_${PROJECT_NAME}"
    COMMENT "Creating distribution package"
    DEPENDS ${PROJECT_NAME}
)

# Print distribution information after build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Distribution package created in: ${DIST_DIR}"
) 