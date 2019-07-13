cmake_minimum_required(VERSION 2.6)

add_definitions(-std=c++11)
find_package(Boost REQUIRED COMPONENTS iostreams program_options filesystem)

add_compile_options(-mno-ms-bitfields)

add_executable(cod_parser_test cod_parser_test.cpp ../../files.cpp)
target_link_libraries(cod_parser_test boost_system boost_iostreams boost_program_options boost_filesystem)
add_custom_command(TARGET cod_parser_test POST_BUILD COMMAND ${CMAKE_COMMAND} ARGS -E copy ${CMAKE_CURRENT_SOURCE_DIR}/test.cod ${CMAKE_CURRENT_BINARY_DIR}/test.cod)
