#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <iostream>
#include <string>
#include "channel.hpp"
#include "logger.h"
#include "multicast-server.h"

namespace PiEar {
    void mainloop_multicast_server(std::vector<channel*> *channels, std::atomic<bool> *click, std::atomic<bool> *kill) {
        boost::asio::io_context io_context;
        boost::asio::ip::udp::endpoint endpoint(boost::asio::ip::address::from_string(MULTICAST_SERVER_GROUP), MULTICAST_SERVER_PORT);
        boost::asio::ip::udp::socket socket(io_context, endpoint.protocol());
        socket.set_option(boost::asio::ip::udp::socket::reuse_address(true));
        socket.set_option(boost::asio::ip::multicast::join_group(endpoint.address()));
        socket.set_option(boost::asio::socket_base::broadcast(true));
        socket.bind(endpoint);
        try {
            PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::info) << "Starting multicast server thread...";
            boost::asio::io_service io_service;
            MulticastServer s(io_service, boost::asio::ip::address::from_string(MULTICAST_SERVER_GROUP), kill, click, channels);
            io_service.run();
        } catch (std::exception& e) {
            PIEAR_LOG_WITH_FILE_LOCATION(boost::log::trivial::error) << "Multicast server failed: " << e.what();
        }
    }
    PiEar::MulticastServer::MulticastServer(boost::asio::io_service& io_service, const boost::asio::ip::address& multicast_address, std::atomic<bool> *kill, std::atomic<bool> *click, std::vector<channel*> *channels):
        endpoint_(multicast_address, MULTICAST_SERVER_PORT), socket_(io_service, endpoint_.protocol()), kill_server(kill), click(click), channels(channels) {
        server_loop(boost::system::error_code());
    }
    void PiEar::MulticastServer::server_loop(const boost::system::error_code& error) {
        std::thread send_click_stream_thread([this] {send_click_stream();});
        auto data_with_channel = new uint16_t[channels->back()->buffer.chunkSize() + 1];
        while(!(*kill_server)) {
            for (auto channel : *channels) {
                if (channel->enabled) {
                    data_with_channel[0] = channel->piear_id;
                    uint16_t *data = channel->buffer.pop();
                    if (data == nullptr) {
                        continue;
                    }
                    memcpy(data_with_channel + 1, data, sizeof(uint16_t) * (channel->buffer.chunkSize()));
                    free(data);
                    mutex_.lock();
                    socket_.send_to(boost::asio::buffer(data_with_channel, sizeof(uint16_t) * (channel->buffer.chunkSize() + 1)), endpoint_);
                    mutex_.unlock();
                }
            }
        }
        delete[] data_with_channel;
        send_click_stream_thread.join();
    }

    void MulticastServer::send_click_stream() {
        auto click_true = new uint16_t[2]{0, 1};
        auto click_false = new uint16_t[2]{0, 0};
        while(!(*kill_server)) {
            if (*click) {
                mutex_.lock();
                socket_.send_to(boost::asio::buffer(click_true, 2 * sizeof(uint16_t)), endpoint_);
                mutex_.unlock();
                while (*click);
            }
        }
        delete[] click_true;
        delete[] click_false;
    }
//    boost::asio::mutable_buffer PiEar::MulticastServer::compress(const uint16_t *stream) {
//        boost::iostreams::stream< boost::iostreams::array_source > source (stream, 128);
//        boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
//        in.push(boost::iostreams::gzip_compressor());
//        std::istringstream uncompressed_string(*stream);
//        in.push(uncompressed_string);
//        std::ostringstream compressed_string;
//        boost::iostreams::copy(in, compressed_string);
//        return compressed_string.str();
//    }
//    boost::asio::mutable_buffer PiEar::MulticastServer::decompress(const uint16_t *stream) {
//        boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
//        in.push(boost::iostreams::gzip_decompressor());
//        std::istringstream compressed_string(*stream);
//        in.push(compressed_string);
//        std::ostringstream uncompressed_string;
//        boost::iostreams::copy(in, uncompressed_string);
//        return uncompressed_string.str();
//    }
}