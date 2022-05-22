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

#include <atomic>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include "channel.hpp"
#include "gen_channels.hpp"
#include "multicast-server.h"

namespace PiEar::Test {

    class receiver {
    public:
        receiver(boost::asio::io_context& io_context, const boost::asio::ip::address& listen_address, std::atomic<bool>& kill)
                : socket_(io_context) , kill_(kill), data_() {
            boost::asio::ip::udp::endpoint listen_endpoint(listen_address, MULTICAST_SERVER_PORT);
            socket_.open(listen_endpoint.protocol());
            socket_.set_option(boost::asio::ip::udp::socket::reuse_address(true));
            socket_.bind(listen_endpoint);
            socket_.set_option(boost::asio::ip::multicast::join_group(boost::asio::ip::address::from_string(MULTICAST_SERVER_GROUP)));
            expectedMessage = (uint16_t*) malloc(sizeof(uint16_t) * 128);
            for (uint16_t i = 0; i < 128; ++i) {
                expectedMessage[i] = i;
            }
            do_receive();
        }
        ~receiver() {
            free(expectedMessage);
        }
    private:
        void do_receive() {
            socket_.async_receive_from(
                boost::asio::buffer(data_), sender_endpoint_,
                [this](boost::system::error_code ec, std::size_t length) {
                    if (!ec) {
                        auto message = (uint16_t*) &data_;
                        if (message[0] == 0) {
                            EXPECT_FALSE(message[1]);
                        } else {
                            EXPECT_EQ(message[0], 1);
                            for (int i = 0; i < 128; i++) {
                                EXPECT_EQ(expectedMessage[i], message[i + 1]);
                            }
                        }
                        if (!kill_) {
                            do_receive();
                        }
                    }
                }
            );
        }
        boost::asio::ip::udp::socket socket_;
        boost::asio::ip::udp::endpoint sender_endpoint_;
        std::array<char, 1024> data_;
        std::atomic<bool>& kill_;
        uint16_t *expectedMessage;
    };

    // Multicast receiver function
    void multicast_receiver(std::atomic<bool> *kill) {
        try {
            boost::asio::io_service io_service;
            receiver r(io_service, boost::asio::ip::address::from_string("0.0.0.0"), *kill);
            io_service.run();
        }
        catch (std::exception &e) {
            std::cerr << "Exception: " << e.what() << "\n";
        }
    }

    TEST(testPiEar, multicast_server) {
        // For right now, send data
        std::vector<PiEar::channel *> *channels = generate_channels(1);
        channels->back()->piear_id = 1;
        std::atomic<bool> click = false;
        std::atomic<bool> kill = false;
        // Fill buffer with data
        for (int _ = 0; _ < 5; _++) {
            auto data = (uint16_t *)malloc(sizeof(uint16_t) * 128);
            for (uint16_t i = 0; i < 128; i++) {
                data[i] = i;
            }
            channels->back()->buffer.push(data);
            free(data);
        }
        std::thread receiverThread(multicast_receiver, &kill);
        std::thread multicastThread(PiEar::mainloop_multicast_server, channels, &click, &kill);
        sleep(1);
        kill = true;
        multicastThread.join();
        receiverThread.join();
        delete channels->back();
        delete channels;
    }

//    TEST(testPiEar, data_compression) {
//        std::string uncompressed = "This is a test message";
//        std::string compressed = PiEar::MulticastServer::compress(&uncompressed);
//        std::string uncompressed2 = PiEar::MulticastServer::decompress(&compressed);
//        EXPECT_EQ(uncompressed, uncompressed2);
//    }
}