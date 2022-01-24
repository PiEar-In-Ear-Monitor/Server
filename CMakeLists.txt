cmake_minimum_required(VERSION 3.21)
project(PiEar_Server VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")
add_executable(PiEar src/multicast-server.cpp)
target_link_libraries(PiEar pthread PocoNet PocoFoundation)

enable_testing()

add_executable(PiEar_Test_click src/click.cpp tests/test_click.cpp)
target_link_libraries(PiEar_Test_click gtest gtest_main)
target_link_libraries(PiEar_Test_click pthread)
add_test(NAME test_click COMMAND PiEar_Test_click)

# add_executable(PiEar_Test_audio tests/test_audio.cpp)
# target_link_libraries(PiEar_Test_audio gtest gtest_main)
# add_test(NAME test_audio COMMAND PiEar_Test_audio)

add_executable(PiEar_Test_multicast_server tests/test_multicast-server.cpp)
target_link_libraries(PiEar_Test_multicast_server gtest gtest_main)
add_test(NAME test_multicast_server COMMAND PiEar_Test_multicast_server)

# add_executable(PiEar_Test_http_server tests/test_http-server.cpp)
# target_link_libraries(PiEar_Test_http_server gtest gtest_main)
# add_test(NAME test_http_server COMMAND PiEar_Test_http_server)
