cmake_minimum_required(VERSION 3.21)
project(PiEar_Server VERSION 1.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-DBOOST_LOG_DYN_LINK")

option(PACKAGE_TESTS "Build the tests" ON)
option(AUDIO_TEST "Build the audio test" OFF)
include_directories(include)
include_directories(helpers)

add_executable(piear src/logger.cpp src/audio.cpp src/click.cpp src/channel.cpp src/http-server.cpp src/multicast-server.cpp src/PiEar-Server.cpp )
target_link_libraries(piear PUBLIC pthread boost_log_setup boost_log boost_thread boost_system boost_iostreams portaudio avutil swresample)
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

    package_add_test(Channel_Test src/channel.cpp src/logger.cpp  tests/test_channel.cpp)
    target_link_libraries(Channel_Test gtest gtest_main pthread boost_log_setup boost_log boost_thread boost_system avutil swresample)

    package_add_test(Multicast_Test src/channel.cpp src/logger.cpp src/multicast-server.cpp tests/test_multicast-server.cpp)
    target_link_libraries(Multicast_Test gtest gtest_main pthread boost_log_setup boost_log boost_thread boost_system boost_iostreams avutil swresample)

    if(AUDIO_TEST)
        package_add_test(Audio_Test src/logger.cpp src/audio.cpp tests/test_audio.cpp)
        target_link_libraries(Audio_Test gtest gtest_main pthread boost_log_setup boost_log boost_thread portaudio)
    endif()

    package_add_test(Http_Server_Test src/channel.cpp src/logger.cpp src/http-server.cpp tests/test_http-server.cpp)
    target_link_libraries(Http_Server_Test gtest gtest_main pthread boost_log_setup boost_log boost_thread boost_system avutil swresample)

    package_add_test(Save_Load_Data_Test src/channel.cpp src/logger.cpp tests/test_save_load_data.cpp)
    target_link_libraries(Save_Load_Data_Test gtest gtest_main boost_log_setup boost_log boost_thread pthread avutil swresample)

    package_add_test(Ring_Buffer_Test src/logger.cpp tests/test_ring_buffer.cpp)
    target_link_libraries(Ring_Buffer_Test gtest gtest_main boost_log_setup boost_log boost_thread pthread)
endif()
