
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Required libraries for linking against nngui (all targets)
set(NNGUI_EXTRA_LIBS "")

# Platform-dependent files for nngui
set(NNGUI_EXTRA_SOURCE "")

include(CheckCXXCompilerFlag)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()
string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # Quench annoying deprecation warnings when compiling GLFW on OSX
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-deprecated-declarations")
endif()

if (MSVC)
  # Disable annoying MSVC warnings (all targets)
  add_definitions(/D "_CRT_SECURE_NO_WARNINGS")

  # Parallel build on MSVC (all targets)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

  if (NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:SSE2")
  endif()
endif()

# Compile with compiler warnings turned on
if(MSVC)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4244 /wd4100 /wd4101 /wd4018 /wd4201 /wd4189 /wd4457 /wd4456 /wd4245 /wd4838 /wd4389 /wd4702")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  CHECK_CXX_COMPILER_FLAG("-std=c++14" HAS_CPP14_FLAG)
  CHECK_CXX_COMPILER_FLAG("-std=c++11" HAS_CPP11_FLAG)

  if (HAS_CPP14_FLAG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
  elseif (HAS_CPP11_FLAG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  else()
    message(FATAL_ERROR "Unsupported compiler -- requires C++11 support!")
  endif()
endif()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

find_package(SDL2 REQUIRED)
find_package(SDL2IMAGE REQUIRED)
find_package(SDL2TTF REQUIRED)
# 
# Library name and options
# 

# Target name
set(target nanogui-sdl)

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

set(include_path "${CMAKE_CURRENT_SOURCE_DIR}/nanogui-sdl/sdlgui")
set(source_path  "${CMAKE_CURRENT_SOURCE_DIR}/nanogui-sdl/sdlgui")

set(headers
  ${include_path}/button.h
  ${include_path}/checkbox.h
  ${include_path}/colorpicker.h
  ${include_path}/colorwheel.h
  ${include_path}/combobox.h
  ${include_path}/common.h
  ${include_path}/dropdownbox.h
  ${include_path}/entypo.h
  ${include_path}/fontstash.h
  ${include_path}/formhelper.h
  ${include_path}/graph.h
  ${include_path}/imagepanel.h
  ${include_path}/imageview.h
  ${include_path}/label.h
  ${include_path}/layout.h
  ${include_path}/messagedialog.h
  ${include_path}/nanort.h
  ${include_path}/nanovg.h
  ${include_path}/nanovg_rt.h
  ${include_path}/popupbutton.h
  ${include_path}/popup.h
  ${include_path}/progressbar.h
  ${include_path}/resources.h
  ${include_path}/screen.h
  ${include_path}/sdlguii.h
  ${include_path}/slider.h
  ${include_path}/stackedwidget.h
  ${include_path}/stb_image.h
  ${include_path}/stb_truetype.h
  ${include_path}/switchbox.h
  ${include_path}/tabheader.h
  ${include_path}/tabwidget.h
  ${include_path}/textbox.h
  ${include_path}/texturebutton.h
  ${include_path}/texturetable.h
  ${include_path}/textureview.h
  ${include_path}/theme.h
  ${include_path}/toolbutton.h
  ${include_path}/vscrollpanel.h
  ${include_path}/widget.h
  ${include_path}/window.h
)

set(sources
  ${source_path}/button.cpp
  ${source_path}/checkbox.cpp
  ${source_path}/colorpicker.cpp
  ${source_path}/colorwheel.cpp
  ${source_path}/combobox.cpp
  ${source_path}/common.cpp
  ${source_path}/dropdownbox.cpp
  ${source_path}/graph.cpp
  ${source_path}/imagepanel.cpp
  ${source_path}/imageview.cpp
  ${source_path}/label.cpp
  ${source_path}/layout.cpp
  ${source_path}/loadimages.cpp
  ${source_path}/messagedialog.cpp
  ${source_path}/nanovg.c
  ${source_path}/popupbutton.cpp
  ${source_path}/popup.cpp
  ${source_path}/progressbar.cpp
  ${source_path}/resources.cpp
  ${source_path}/screen.cpp
  ${source_path}/slider.cpp
  ${source_path}/stackedwidget.cpp
  ${source_path}/switchbox.cpp
  ${source_path}/tabheader.cpp
  ${source_path}/tabwidget.cpp
  ${source_path}/textbox.cpp
  ${source_path}/texturebutton.cpp
  ${source_path}/textureview.cpp
  ${source_path}/theme.cpp
  ${source_path}/vscrollpanel.cpp
  ${source_path}/widget.cpp
  ${source_path}/window.cpp
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

target_compile_features(${target} PUBLIC cxx_std_14)

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
    ${PROJECT_BINARY_DIR}/nanogui-sdl
    ${CMAKE_CURRENT_SOURCE_DIR}/nanogui-sdl
    ${CMAKE_CURRENT_BINARY_DIR}/nanogui-sdl

    PUBLIC
    ${DEFAULT_INCLUDE_DIRECTORIES}

    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/nanogui-sdl>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/nanogui-sdl>
    $<INSTALL_INTERFACE:nanogui-sdl>
)


# 
# Libraries
# 

target_link_libraries(${target}
    PRIVATE
    ${SDL2_LIBRARY}
    ${SDL2TTF_LIBRARY}
    ${SDL2IMAGE_LIBRARY}
    PUBLIC
    ${DEFAULT_LIBRARIES}
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


add_executable(nanogui-sdl-example1 ${CMAKE_CURRENT_SOURCE_DIR}/nanogui-sdl/example1.cpp )
set_target_properties(
    nanogui-sdl-example1 PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build/debug")
target_link_libraries(nanogui-sdl-example1 nanogui-sdl ${SDL2_LIBRARY} ${SDL2IMAGE_LIBRARY} ${SDL2TTF_LIBRARY})

add_executable(nanogui-sdl-twoscreens ${CMAKE_CURRENT_SOURCE_DIR}/nanogui-sdl/twoscreens.cpp )
set_target_properties(
    nanogui-sdl-twoscreens PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build/debug")
target_link_libraries(nanogui-sdl-twoscreens nanogui-sdl ${SDL2_LIBRARY} ${SDL2IMAGE_LIBRARY} ${SDL2TTF_LIBRARY})


