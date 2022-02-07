cmake_minimum_required(VERSION 3.21)
project(PiEar_Server VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")
add_executable(PiEar ${SOURCES})
target_link_libraries(PiEar pthread PocoNet PocoFoundation sqlite3)

enable_testing()

add_executable(PiEar_Test_click src/click.cpp tests/test_click.cpp)
target_link_libraries(PiEar_Test_click gtest gtest_main pthread)
add_test(NAME test_click COMMAND PiEar_Test_click)

# add_executable(PiEar_Test_audio tests/test_audio.cpp)
# target_link_libraries(PiEar_Test_audio gtest gtest_main)
# add_test(NAME test_audio COMMAND PiEar_Test_audio)

add_executable(PiEar_Test_multicast_server tests/test_multicast-server.cpp)
target_link_libraries(PiEar_Test_multicast_server gtest gtest_main)
add_test(NAME test_multicast_server COMMAND PiEar_Test_multicast_server)

add_executable(PiEar_Test_task src/task.cpp tests/test_task.cpp)
target_link_libraries(PiEar_Test_task gtest gtest_main)
add_test(NAME test_task COMMAND PiEar_Test_task)

add_executable(PiEar_Test_json_data src/json_data.cpp tests/test_json_data.cpp)
target_link_libraries(PiEar_Test_json_data gtest gtest_main)
add_test(NAME test_json_data COMMAND PiEar_Test_json_data)

# add_executable(PiEar_Test_http_server tests/test_http-server.cpp)
# target_link_libraries(PiEar_Test_http_server gtest gtest_main)
# add_test(NAME test_http_server COMMAND PiEar_Test_http_server)

add_executable(PiEar_Test_new_feature tests/test.cpp)
target_link_libraries(PiEar_Test_new_feature sqlite3)
add_test(NAME test_new_feature COMMAND PiEar_Test_new_featue)
