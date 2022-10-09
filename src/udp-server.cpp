#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <string>
#include "channel.h"
#include "logger.h"
#include "udp-server.h"

namespace PiEar {
    void mainloop_udp_server(std::vector<channel*> *channels, std::atomic<bool> *click, std::atomic<bool> *kill) {
        try {
            boost::asio::io_context io_context;
            UdpServer s(io_context, kill, click, channels);
            io_context.run();
        } catch (std::exception& e) {
            PIEAR_LOG_WITH_FILE_LOCATION(boost::log::trivial::error) << "Multicast server failed: " << e.what();
        }
    }
    PiEar::UdpServer::UdpServer(boost::asio::io_context& io_context, std::atomic<bool> *kill, std::atomic<bool> *click, std::vector<channel*> *channels):
            socket_(io_context, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), UDP_SERVER_PORT)), kill_server(kill), click(click), channels(channels) {
        server_loop();
    }
    void PiEar::UdpServer::server_loop() {
//        std::thread send_click_stream_thread([this] {send_click_stream();});
        auto data_with_channel = new uint16_t[BUFFER_CHUNK_SIZE + 1];
        bool setup_done = false;
        PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::trace) << "Multicast server waiting for channels to be setup";
        while (!setup_done) {
            setup_done = true;
            for (auto channel : *channels) {
                if (!channel->swr_ctx_init()) {
                    setup_done = false;
                }
            }
        }
        int bufferBufferSize = sizeof(uint16_t) * (BUFFER_CHUNK_SIZE + 1);
        socket_.receive_from(boost::asio::buffer(data_with_channel, bufferBufferSize), endpoint_);
        PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::trace) << "Multicast server ready";
        while(!(*kill_server)) {
            for (auto channel : *channels) {
                if (channel->enabled) {
                    data_with_channel[0] = channel->piear_id;
                    PiEar::return_data data = channel->get_sample();
                    if (data.data == nullptr) {
                        continue;
                    }
                    memcpy(data_with_channel + 1, data.data, sizeof(uint16_t) * data.frames);
                    mutex_.lock();
                    socket_.send_to(boost::asio::buffer(data_with_channel, bufferBufferSize), endpoint_);
                    mutex_.unlock();
                }
            }
        }
        delete[] data_with_channel;
//        send_click_stream_thread.join();
    }
    void UdpServer::send_click_stream() {
        auto click_true = new uint16_t[2]{0, 1};
        auto click_false = new uint16_t[2]{0, 0};
        while(!(*kill_server)) {
            if (*click) {
                mutex_.lock();
                socket_.send_to(boost::asio::buffer(click_true, 2 * sizeof(uint16_t)), endpoint_);
                mutex_.unlock();
                while (*click) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
        }
        delete[] click_true;
        delete[] click_false;
    }
//    boost::asio::mutable_buffer PiEar::UdpServer::compress(const uint16_t *stream) {
//        boost::iostreams::stream< boost::iostreams::array_source > source (stream, 128);
//        boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
//        in.push(boost::iostreams::gzip_compressor());
//        std::istringstream uncompressed_string(*stream);
//        in.push(uncompressed_string);
//        std::ostringstream compressed_string;
//        boost::iostreams::copy(in, compressed_string);
//        return compressed_string.str();
//    }
//    boost::asio::mutable_buffer PiEar::UdpServer::decompress(const uint16_t *stream) {
//        boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
//        in.push(boost::iostreams::gzip_decompressor());
//        std::istringstream compressed_string(*stream);
//        in.push(compressed_string);
//        std::ostringstream uncompressed_string;
//        boost::iostreams::copy(in, uncompressed_string);
//        return uncompressed_string.str();
//    }
}