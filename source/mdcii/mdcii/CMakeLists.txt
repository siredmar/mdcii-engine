
# 
# External dependencies
# 

# find_package(THIRDPARTY REQUIRED)
find_package(SDL2)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-psabi")
find_package(Boost
    1.65.1                         # Minimum or EXACT version e.g. 1.36.0
    REQUIRED                       # Fail with error if Boost is not found
    COMPONENTS  regex system iostreams filesystem 
)

# 
# Library name and options
# 

# Target name
set(target mdcii)

# Exit here if required dependencies are not met
message(STATUS "Lib ${target}")

# Set API export file and macro
string(MAKE_C_IDENTIFIER ${target} target_id)
string(TOUPPER ${target_id} target_id)
set(feature_file         "include/${target}/${target}_features.h")
set(export_file          "include/${target}/${target}_export.h")
set(template_export_file "include/${target}/${target}_api.h")
set(export_macro         "${target_id}_API")


# 
# Sources
# 

set(include_path "${CMAKE_CURRENT_SOURCE_DIR}/include/${target}")
set(source_path  "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(headers
    ${include_path}/mdcii.hpp
    ${include_path}/common/stringhelpers.hpp
    ${include_path}/animation.hpp
    ${include_path}/framebuffer/framebuffer.hpp
    ${include_path}/framebuffer/framebuffer_pal8.hpp
    ${include_path}/framebuffer/framebuffer_rgb24.hpp
    ${include_path}/framebuffer/framebuffer_trans_pal8.hpp
    ${include_path}/framebuffer/palette.hpp
    ${include_path}/bsh/genericbshreader.hpp
    ${include_path}/bsh/bshreader.hpp
    ${include_path}/bsh/bshwriter.hpp
    ${include_path}/bsh/bshtexture.hpp
    ${include_path}/bsh/zeireader.hpp
    ${include_path}/bsh/zeitexture.hpp
    ${include_path}/files/files.hpp
    ${include_path}/files/filestocheck.hpp
    ${include_path}/savegames/savegames.hpp
    ${include_path}/savegames/scenarios.hpp
    ${include_path}/cache/cacheprotobuf.hpp
    ${include_path}/renderer/renderer.hpp
    ${include_path}/menu/mainmenu.hpp
    ${include_path}/menu/singleplayerwindow.hpp
    ${include_path}/menu/startgamewindow.hpp
    ${include_path}/menu/gamewindow.hpp
    ${include_path}/menu/fps.hpp
    ${include_path}/menu/scale.hpp
    ${include_path}/cod/codhelpers.hpp
    ${include_path}/cod/cod_parser.hpp
    ${include_path}/cod/basegad_dat.hpp
    ${include_path}/cod/buildings.hpp
    ${include_path}/cod/host_gad.hpp
    ${include_path}/cod/mission_gad.hpp
    ${include_path}/cod/text_cod.hpp
    ${include_path}/gam/chunk.hpp
    ${include_path}/gam/city.hpp
    ${include_path}/gam/deer.hpp
    ${include_path}/gam/island.hpp
    ${include_path}/gam/island3.hpp
    ${include_path}/gam/island4.hpp
    ${include_path}/gam/island5.hpp
    ${include_path}/gam/islandhouse.hpp
    ${include_path}/gam/marketplace.hpp
    ${include_path}/gam/military.hpp
    ${include_path}/gam/missions.hpp
    ${include_path}/gam/productionlist.hpp
    ${include_path}/gam/rawgrowth.hpp
    ${include_path}/gam/scene.hpp
    ${include_path}/gam/settlers.hpp
    ${include_path}/gam/shipyard.hpp
    ${include_path}/gam/warehouse.hpp
    ${include_path}/gam/gam_parser.hpp
    ${include_path}/world/world.hpp
    ${include_path}/version/version.hpp
    ${include_path}/block.hpp
    ${include_path}/insel.hpp
    ${include_path}/camera/camera.hpp
    ${include_path}/camera/bshresources.hpp
    ${include_path}/karte.hpp
    ${include_path}/spielbildschirm.hpp
    ${include_path}/strukturen.hpp
    ${include_path}/welt.hpp
    ${include_path}/sdl2shared/sdl2shared.hpp
)

set(sources
    ${source_path}/mdcii.cpp
    ${source_path}/common/stringhelpers.cpp
    ${source_path}/framebuffer/framebuffer.cpp
    ${source_path}/framebuffer/framebuffer_pal8.cpp
    ${source_path}/framebuffer/framebuffer_rgb24.cpp
    ${source_path}/framebuffer/framebuffer_trans_pal8.cpp
    ${source_path}/framebuffer/palette.cpp
    ${source_path}/files/files.cpp
    ${source_path}/bsh/bshwriter.cpp
    ${source_path}/bsh/bshtexture.cpp
    ${source_path}/bsh/zeitexture.cpp
    ${source_path}/savegames/savegames.cpp
    ${source_path}/savegames/scenarios.cpp
    ${source_path}/renderer/renderer.cpp
    ${source_path}/menu/mainmenu.cpp
    ${source_path}/menu/singleplayerwindow.cpp
    ${source_path}/menu/startgamewindow.cpp
    ${source_path}/menu/gamewindow.cpp
    ${source_path}/menu/scale.cpp
    ${source_path}/cod/codhelpers.cpp
    ${source_path}/cod/cod_parser.cpp
    ${source_path}/cod/basegad_dat.cpp
    ${source_path}/cod/buildings.cpp
    ${source_path}/cod/host_gad.cpp
    ${source_path}/cod/mission_gad.cpp
    ${source_path}/cod/text_cod.cpp
    ${source_path}/gam/chunk.cpp
    ${source_path}/gam/city.cpp
    ${source_path}/gam/deer.cpp
    ${source_path}/gam/island3.cpp
    ${source_path}/gam/island4.cpp
    ${source_path}/gam/island5.cpp
    ${source_path}/gam/islandhouse.cpp
    ${source_path}/gam/marketplace.cpp
    ${source_path}/gam/military.cpp
    ${source_path}/gam/missions.cpp
    ${source_path}/gam/productionlist.cpp
    ${source_path}/gam/rawgrowth.cpp
    ${source_path}/gam/scene.cpp
    ${source_path}/gam/settlers.cpp
    ${source_path}/gam/shipyard.cpp
    ${source_path}/gam/warehouse.cpp
    ${source_path}/gam/gam_parser.cpp
    ${source_path}/version/version.cpp
    ${source_path}/world/world.cpp
    ${source_path}/block.cpp
    ${source_path}/insel.cpp
    ${source_path}/camera/camera.cpp
    ${source_path}/camera/bshresources.cpp
    ${source_path}/karte.cpp
    ${source_path}/spielbildschirm.cpp
    ${source_path}/strukturen.cpp
    ${source_path}/welt.cpp
)

# Group source files
set(header_group "Header Files (API)")
set(source_group "Source Files")
source_group_by_path(${include_path} "\\\\.h$|\\\\.hpp$" 
    ${header_group} ${headers})
source_group_by_path(${source_path}  "\\\\.cpp$|\\\\.c$|\\\\.h$|\\\\.hpp$" 
    ${source_group} ${sources})


# 
# Create library
# 

# Build library
add_library(${target} STATIC
    ${sources}
    ${headers}
)

target_compile_features(${target} PUBLIC cxx_std_17)

# Create namespaced alias
add_library(${META_PROJECT_NAME}::${target} ALIAS ${target})

# Export library for downstream projects
export(TARGETS ${target} NAMESPACE ${META_PROJECT_NAME}:: FILE ${PROJECT_BINARY_DIR}/cmake/${target}/${target}-export.cmake)

# Create feature detection header
# Compilers: https://cmake.org/cmake/help/v3.1/variable/CMAKE_LANG_COMPILER_ID.html#variable:CMAKE_%3CLANG%3E_COMPILER_ID
# Feature: https://cmake.org/cmake/help/v3.1/prop_gbl/CMAKE_CXX_KNOWN_FEATURES.html

# Check for availability of module; use pre-generated version if not found
if (WriterCompilerDetectionHeaderFound)
    write_compiler_detection_header(
        FILE ${feature_file}
        PREFIX ${target_id}
        COMPILERS AppleClang Clang GNU MSVC
        FEATURES cxx_alignas cxx_alignof cxx_constexpr cxx_final cxx_noexcept cxx_nullptr cxx_sizeof_member cxx_thread_local
        VERSION 3.2
    )
else()
    file(
        COPY ${PROJECT_SOURCE_DIR}/source/codegeneration/${target}_features.h
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include/${target}
        USE_SOURCE_PERMISSIONS
    )
endif()

# Create API export header
generate_export_header(${target}
    EXPORT_FILE_NAME  ${export_file}
    EXPORT_MACRO_NAME ${export_macro}
)

# 
# Project options
# 

set_target_properties(${target}
    PROPERTIES
    ${DEFAULT_PROJECT_OPTIONS}
    FOLDER "${IDE_FOLDER}"
    VERSION ${META_VERSION}
    SOVERSION ${META_VERSION_MAJOR}
)


# 
# Include directories
# 

target_include_directories(${target}
    PRIVATE
    ${PROJECT_BINARY_DIR}/source/include/${target}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${target}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${target}
    ${CMAKE_CURRENT_BINARY_DIR}/include/${target}
    PUBLIC
    ${DEFAULT_INCLUDE_DIRECTORIES}

    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/mdcii>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/mdcii>
    $<INSTALL_INTERFACE:include>
)


# 
# Libraries
# 

target_link_libraries(${target}
    PRIVATE
    ${META_PROJECT_NAME}::nanogui-sdl
    ${Boost_LIBRARIES}
    
    PUBLIC
    ${DEFAULT_LIBRARIES}
    ${SDL2_LIBRARY}
    ${META_PROJECT_NAME}::PlatformFolders
    ${META_PROJECT_NAME}::proto-cod
    ${META_PROJECT_NAME}::proto-gamelist
    ${META_PROJECT_NAME}::proto-textcod
    stdc++fs
    INTERFACE
)


# 
# Compile definitions
# 

target_compile_definitions(${target}
    PRIVATE

    PUBLIC
    $<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:${target_id}_STATIC_DEFINE>
    ${DEFAULT_COMPILE_DEFINITIONS}

    INTERFACE
)


# 
# Compile options
# 

target_compile_options(${target}
    PRIVATE

    PUBLIC
    ${DEFAULT_COMPILE_OPTIONS}

    INTERFACE
)


# 
# Linker options
# 

target_link_libraries(${target}
    PRIVATE

    PUBLIC
    ${DEFAULT_LINKER_OPTIONS}

    INTERFACE
)


#
# Target Health
#

perform_health_checks(
    ${target}
    ${sources}
    ${headers}
)


# 
# Deployment
# 

# Library
install(TARGETS ${target}
    EXPORT  "${target}-export"            COMPONENT dev
    RUNTIME DESTINATION ${INSTALL_BIN}    COMPONENT runtime
    LIBRARY DESTINATION ${INSTALL_SHARED} COMPONENT runtime
    ARCHIVE DESTINATION ${INSTALL_LIB}    COMPONENT dev
)

# Header files
install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${target} DESTINATION ${INSTALL_INCLUDE}
    COMPONENT dev
)

# Generated header files
install(DIRECTORY
    ${CMAKE_CURRENT_BINARY_DIR}/include/${target} DESTINATION ${INSTALL_INCLUDE}
    COMPONENT dev
)

# CMake config
install(EXPORT ${target}-export
    NAMESPACE   ${META_PROJECT_NAME}::
    DESTINATION ${INSTALL_CMAKE}/${target}
    COMPONENT   dev
)