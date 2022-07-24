cmake_minimum_required(VERSION 3.21)
project(PiEar_Server VERSION 1.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(PACKAGE_TESTS "Build the tests" ON)
option(AUDIO_TEST "Build the audio test" OFF)
include_directories(include)
include_directories(helpers)
add_executable(piear src/audio.cpp src/click.cpp src/http-server.cpp src/multicast-server.cpp src/PiEar-Server.cpp)
target_link_libraries(piear PUBLIC pthread boost_filesystem boost_system boost_iostreams portaudio)
message(STATUS "Test: ${CMAKE_INSTALL_PREFIX}")
install(TARGETS piear DESTINATION bin)
install(DIRECTORY webserver DESTINATION share/piear USE_SOURCE_PERMISSIONS)

if(PACKAGE_TESTS)
    enable_testing()
    include(GoogleTest)
    macro(package_add_test TEST_NAME)
        add_executable(${TEST_NAME} ${ARGN})
        gtest_discover_tests(${TEST_NAME})
    endmacro()

    package_add_test(Click_Test src/click.cpp tests/test_click.cpp)
    target_link_libraries(Click_Test gtest gtest_main pthread)

    package_add_test(Channel_Test tests/test_channel.cpp)
    target_link_libraries(Channel_Test gtest gtest_main pthread)

    package_add_test(Multicast_Test src/multicast-server.cpp tests/test_multicast-server.cpp)
    target_link_libraries(Multicast_Test gtest gtest_main pthread boost_system boost_filesystem boost_iostreams)

    if(AUDIO_TEST)
        package_add_test(Audio_Test src/audio.cpp tests/test_audio.cpp)
        target_link_libraries(Audio_Test gtest gtest_main pthread portaudio)
    endif()

    package_add_test(Http_Server_Test src/http-server.cpp tests/test_http-server.cpp)
    target_link_libraries(Http_Server_Test gtest gtest_main pthread boost_system)

    package_add_test(Save_Load_Data_Test tests/test_save_load_data.cpp)
    target_link_libraries(Save_Load_Data_Test gtest gtest_main pthread)

    package_add_test(Ring_Buffer_Test tests/test_ring_buffer.cpp)
    target_link_libraries(Ring_Buffer_Test gtest gtest_main pthread)
endif()
