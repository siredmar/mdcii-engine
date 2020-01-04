cmake_minimum_required(VERSION 2.6)

add_definitions(-std=c++14)
find_package(Boost REQUIRED COMPONENTS iostreams program_options filesystem)
find_package(SDL REQUIRED)

add_subdirectory(proto)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

add_compile_options(-mno-ms-bitfields)

add_executable(mdcii-inselbmp inselbmp.cpp bildspeicher.cpp bildspeicher_pal8.cpp block.cpp insel.cpp strukturen.cpp files.cpp version.cpp  cod_parser.cpp)
target_link_libraries(mdcii-inselbmp boost_system boost_iostreams boost_regex boost_filesystem cod_proto protobuf)

add_executable(mdcii-weltbmp weltbmp.cpp bildspeicher.cpp bildspeicher_pal8.cpp block.cpp insel.cpp strukturen.cpp welt.cpp files.cpp version.cpp cod_parser.cpp)
target_link_libraries(mdcii-weltbmp boost_system boost_iostreams boost_filesystem boost_regex cod_proto protobuf)

add_executable(mdcii-sdltest sdltest.cpp bildspeicher.cpp bildspeicher_pal8.cpp block.cpp insel.cpp strukturen.cpp welt.cpp kamera.cpp karte.cpp spielbildschirm.cpp files.cpp version.cpp cod_parser.cpp)
target_link_libraries(mdcii-sdltest SDL boost_system boost_iostreams boost_program_options boost_filesystem boost_regex cod_proto protobuf)
if(WIN32)
  target_link_libraries(mdcii-sdltest mingw32 winmm dxguid SDLmain)
endif(WIN32)

add_executable(mdcii-bshdump bshdump.cpp bildspeicher.cpp bildspeicher_pal8.cpp bildspeicher_rgb24.cpp)
target_link_libraries(mdcii-bshdump boost_iostreams boost_program_options)

add_executable(mdcii-bshpacker bshpacker.cpp bsh_schreiber.cpp)
target_link_libraries(mdcii-bshpacker boost_iostreams boost_program_options)

add_executable(mdcii-zeitext zeitext.cpp bildspeicher.cpp bildspeicher_pal8.cpp bildspeicher_rgb24.cpp)
target_link_libraries(mdcii-zeitext boost_iostreams boost_program_options)

add_executable(mdcii-codcat codcat.cpp)
target_link_libraries(mdcii-codcat boost_iostreams)

add_executable(cod_parser_test test/cod_parser_test/cod_parser_test.cpp files.cpp cod_parser.cpp)
target_link_libraries(cod_parser_test boost_system boost_iostreams boost_program_options boost_filesystem boost_regex cod_proto protobuf)
