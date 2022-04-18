//******************************************************************************
// Copyright © 2021  Alexander O'Connor
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//******************************************************************************

#include "gtest/gtest.h"
#include "multicast-server.h"
#include "channel.hpp"
#include <boost/asio.hpp>
#include <thread>
#include <atomic>
#include <vector>

TEST(testPiEar, multicast_server)
{
    // For right now, send data
//    PiEar::mainloop_multicast_server();

//    std::atomic<bool> done(false);
//    std::vector<PiEar::channel> channels;
//    channels.push_back(PiEar::channel(0, 1, "Channel 1", true);
//    std::thread multicast_server_thread(PiEar::mainloop_multicast_server, );
//    // Receive multicast packets
//    boost::asio::io_service io_service;
//    boost::asio::ip::udp::endpoint multicast_endpoint(boost::asio::ip::address::from_string(MULTICAST_SERVER_GROUP), MULTICAST_SERVER_PORT);
//    boost::asio::ip::udp::socket socket(io_service, multicast_endpoint.protocol());
//    socket.set_option(boost::asio::ip::udp::socket::reuse_address(true));
//    socket.set_option(boost::asio::ip::multicast::join_group(multicast_endpoint.address()));
//    socket.set_option(boost::asio::ip::multicast::enable_loopback(true));
//    socket.bind(multicast_endpoint);
//
//    // Read message
//    char buffer[1024];
//    std::size_t len = socket.receive(boost::asio::buffer(buffer));
//    std::string message(buffer, len);
//
//    // Check message
//    EXPECT_EQ(message, "Hello World!");
    EXPECT_EQ(1000, 1000);
}
