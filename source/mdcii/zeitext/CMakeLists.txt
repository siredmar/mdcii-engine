#
# External dependencies
#
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-psabi")

SET(Boost_USE_STATIC_LIBS OFF)
SET(Boost_USE_MULTITHREAD ON)
find_package(Boost
    1.65.1                         # Minimum or EXACT version e.g. 1.36.0
    REQUIRED                       # Fail with error if Boost is not found
    COMPONENTS  regex system iostreams program_options filesystem 
)
find_package(SDL2)

# Target name
set(target zeitext)

# Exit here if required dependencies are not met
message(STATUS "Application ${target}")

set(source_path  "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(sources
    ${source_path}/zeitext.cpp
)

#
# Create executable
#

# Build executable
add_executable(${target}
    ${sources}
)

# Create namespaced alias
add_executable(${META_PROJECT_NAME}::${target} ALIAS ${target})


#
# Project options
#

set_target_properties(${target}
    PROPERTIES
    ${DEFAULT_PROJECT_OPTIONS}
    FOLDER "${IDE_FOLDER}"
)


#
# Include directories
#

target_include_directories(${target}
    PRIVATE
    ${DEFAULT_INCLUDE_DIRECTORIES}
    ${PROJECT_BINARY_DIR}/source/include
)


#
# Libraries
#

target_link_libraries(${target}
    PRIVATE
    ${DEFAULT_LIBRARIES}
    ${SDL2_LIBRARIES}
    ${Boost_LIBRARIES}
    ${META_PROJECT_NAME}::mdcii
    
    PUBLIC
)

#
# Compile definitions
#

target_compile_definitions(${target}
    PRIVATE
    ${DEFAULT_COMPILE_DEFINITIONS}
)


#
# Compile options
#

target_compile_options(${target}
    PRIVATE
    ${DEFAULT_COMPILE_OPTIONS}
)


#
# Linker options
#

target_link_libraries(${target}
    PRIVATE
    ${DEFAULT_LINKER_OPTIONS}
)


#
# Target Health
#

perform_health_checks(
    ${target}
    ${sources}
)


#
# Deployment
#

# Executable
install(TARGETS ${target}
    RUNTIME DESTINATION ${INSTALL_BIN} COMPONENT app
    BUNDLE  DESTINATION ${INSTALL_BIN} COMPONENT app
)
