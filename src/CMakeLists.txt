cmake_minimum_required(VERSION 2.6)

add_definitions(-std=c++17)
find_package(Boost REQUIRED COMPONENTS iostreams program_options)
find_package(SDL REQUIRED)

add_compile_options(-mno-ms-bitfields)

add_executable(mdcii-inselbmp inselbmp.cpp bildspeicher.cpp bildspeicher_pal8.cpp block.cpp insel.cpp strukturen.cpp bebauung.cpp grafiken.cpp files.cpp)
target_link_libraries(mdcii-inselbmp boost_iostreams stdc++fs)

add_executable(mdcii-weltbmp weltbmp.cpp bildspeicher.cpp bildspeicher_pal8.cpp block.cpp insel.cpp strukturen.cpp welt.cpp bebauung.cpp grafiken.cpp files.cpp)
target_link_libraries(mdcii-weltbmp boost_iostreams stdc++fs)

add_executable(mdcii-sdltest sdltest.cpp bildspeicher.cpp bildspeicher_pal8.cpp block.cpp insel.cpp strukturen.cpp welt.cpp kamera.cpp karte.cpp spielbildschirm.cpp bebauung.cpp grafiken.cpp files.cpp)
target_link_libraries(mdcii-sdltest SDL boost_iostreams boost_program_options stdc++fs)
if(WIN32)
  target_link_libraries(mdcii-sdltest mingw32 winmm dxguid SDLmain)
endif(WIN32)
add_custom_command(TARGET mdcii-sdltest POST_BUILD COMMAND ${CMAKE_COMMAND} ARGS -E copy ${CMAKE_CURRENT_SOURCE_DIR}/bebauung.txt ${CMAKE_CURRENT_BINARY_DIR}/bebauung.txt)
add_custom_command(TARGET mdcii-sdltest POST_BUILD COMMAND ${CMAKE_COMMAND} ARGS -E copy ${CMAKE_CURRENT_SOURCE_DIR}/grafiken.txt ${CMAKE_CURRENT_BINARY_DIR}/grafiken.txt)

add_executable(mdcii-bshdump bshdump.cpp bildspeicher.cpp bildspeicher_pal8.cpp bildspeicher_rgb24.cpp)
target_link_libraries(mdcii-bshdump boost_iostreams boost_program_options)

add_executable(mdcii-bshpacker bshpacker.cpp bsh_schreiber.cpp)
target_link_libraries(mdcii-bshpacker boost_iostreams boost_program_options)

add_executable(mdcii-zeitext zeitext.cpp bildspeicher.cpp bildspeicher_pal8.cpp bildspeicher_rgb24.cpp)
target_link_libraries(mdcii-zeitext boost_iostreams boost_program_options)

add_executable(mdcii-codcat codcat.cpp)
target_link_libraries(mdcii-codcat boost_iostreams)
