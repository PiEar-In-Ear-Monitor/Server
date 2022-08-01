#include <atomic>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <gtest/gtest.h>
#include <iostream>
#include <thread>
#include <vector>
#include "channel.h"
#include "gen_channels.hpp"
#include "multicast-server.h"

enum {
MESSAGES_TO_SEND = 5
};

namespace PiEar::Test {
    class receiver {
    public:
        receiver(boost::asio::io_context& io_context, const boost::asio::ip::address& listen_address, int message_count)
                : socket_(io_context), data_() {
            boost::asio::ip::udp::endpoint listen_endpoint(listen_address, MULTICAST_SERVER_PORT);
            socket_.open(listen_endpoint.protocol());
            socket_.set_option(boost::asio::ip::udp::socket::reuse_address(true));
            socket_.bind(listen_endpoint);
            socket_.set_option(boost::asio::ip::multicast::join_group(boost::asio::ip::address::from_string(MULTICAST_SERVER_GROUP)));
            expectedMessage = (uint16_t*) malloc(sizeof(uint16_t) * 128);
            for (uint16_t i = 0; i < 128; ++i) {
                expectedMessage[i] = i;
            }
            for (int i = 0; i < message_count; i++) {
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
                            }
                        }
                );
            }
        }
        ~receiver() {
            free(expectedMessage);
        }
    private:
        boost::asio::ip::udp::socket socket_;
        boost::asio::ip::udp::endpoint sender_endpoint_;
        std::array<char, 1024> data_;
        uint16_t *expectedMessage;
    };
    void multicast_receiver(int message_count) {
        try {
            boost::asio::io_service io_service;
            receiver r(io_service, boost::asio::ip::address::from_string("0.0.0.0"), message_count);
            io_service.run();
        }
        catch (std::exception &e) {
            std::cerr << "Exception: " << e.what() << "\n";
        }
    }
    TEST(testPiEar, multicast_server) {
        std::vector<PiEar::channel *> *channels = generate_channels(1);
        channels->back()->piear_id = 1;
        std::atomic<bool> click = false;
        std::atomic<bool> kill = false;
        auto data = (uint16_t *)malloc(sizeof(uint16_t) * 128);
        for (int _ = 0; _ < MESSAGES_TO_SEND; _++) {
            for (uint16_t i = 0; i < 128; i++) {
                data[i] = i;
            }
            channels->back()->buffer.push(data);
        }
        free(data);
        std::thread receiverThread(multicast_receiver, MESSAGES_TO_SEND);
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
