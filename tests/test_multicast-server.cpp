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
#include "boost/bind.hpp"

namespace PiEar::Test {
    std::vector<std::string> expectedMessages{
        "Message 4",
        "Message 3",
        "Message 2",
        "Message 1",
        "Message 0",
    };

    class receiver {
    public:
        receiver(boost::asio::io_service &io_service, const boost::asio::ip::address &listen_address,
                 std::atomic<bool> *kill)
                : socket_(io_service), kill(kill) {
            // Create the socket so that multiple may be bound to the same address.
            boost::asio::ip::udp::endpoint listen_endpoint(listen_address, MULTICAST_SERVER_PORT);
            socket_.open(listen_endpoint.protocol());
            socket_.set_option(boost::asio::ip::udp::socket::reuse_address(true));
            socket_.bind(listen_endpoint);

            // Join the multicast group.
            socket_.set_option(boost::asio::ip::multicast::join_group(listen_address));

            socket_.async_receive_from(
                boost::asio::buffer(data_, max_length),
                sender_endpoint_,
               boost::bind(&receiver::handle_receive_from, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
            );
        }

        void handle_receive_from(const boost::system::error_code &error, size_t bytes_recvd) {
            if (!error && !(*kill)) {
                std::string message(data_, bytes_recvd);
                EXPECT_EQ(message, expectedMessages.back());
                expectedMessages.pop_back();

                socket_.async_receive_from(
                    boost::asio::buffer(data_, max_length),
                    sender_endpoint_,
                    boost::bind(&receiver::handle_receive_from, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
                );
            }
        }

    private:
        std::atomic<bool> *kill;
        boost::asio::ip::udp::socket socket_;
        boost::asio::ip::udp::endpoint sender_endpoint_;
        enum { max_length = 1024 };
        char data_[max_length]{};
    };


    // Multicast receiver function
    void multicast_receiver(std::atomic<bool> *kill) {
        try {
            boost::asio::io_service io_service;
            receiver r(io_service, boost::asio::ip::address::from_string(MULTICAST_SERVER_GROUP), kill);
            io_service.run();
        }
        catch (std::exception &e) {
            std::cerr << "Exception: " << e.what() << "\n";
        }
    }

    TEST(testPiEar, multicast_server) {
        // For right now, send data
        std::vector<PiEar::channel *> channels;
        channels.push_back(new PiEar::channel(0, 1, "Channel 1", true));
        std::atomic<bool> click = false;
        std::atomic<bool> kill = false;
        std::thread multicastThread(PiEar::mainloop_multicast_server, &channels, &click, &kill);
        std::thread receiverThread(multicast_receiver, &kill);
        sleep(5);
        kill = true;
        multicastThread.join();
        receiverThread.join();
    }
}