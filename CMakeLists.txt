cmake_minimum_required(VERSION 3.21)
project(PiEar_Server VERSION 1.0)
set(CMAKE_CXX_STANDARD 20)

option(PACKAGE_TESTS "Build the tests" ON)

include_directories(include)
include_directories(helpers)
file(GLOB SOURCES "src/*.cpp")
add_executable(PiEar ${SOURCES})
target_link_libraries(PiEar pthread boost_filesystem boost_system)

if(PACKAGE_TESTS)
    enable_testing()
    include(GoogleTest)
    macro(package_add_test TEST_NAME)
        add_executable(${TEST_NAME} ${ARGN})
        target_link_libraries(${TEST_NAME} gtest gtest_main pthread boost_filesystem boost_system)
        gtest_discover_tests(${TEST_NAME})
    endmacro()

    package_add_test(Click_Test src/click.cpp tests/test_click.cpp)
    package_add_test(Channel_Test tests/test_channel.cpp)
    package_add_test(Multicast_Test src/multicast-server.cpp tests/test_multicast-server.cpp)
    package_add_test(Audio_Test src/audio.cpp tests/test_audio.cpp)
    package_add_test(Http_Server_Test src/http-server.cpp tests/test_http-server.cpp)
#    package_add_test(Save_Load_Data_Test tests/test_save_load_data.cpp)
endif()

