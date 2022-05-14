#include "multicast-server.h"
#include "channel.hpp"
#include <atomic>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/copy.hpp>

namespace PiEar {
    void mainloop_multicast_server(std::vector<channel*> *audio_streams, std::atomic<bool> *click_stream, std::atomic<bool> *end)
    {
        boost::asio::io_context io_context;
        boost::asio::ip::udp::endpoint endpoint(boost::asio::ip::address::from_string(MULTICAST_SERVER_GROUP), MULTICAST_SERVER_PORT);
        boost::asio::ip::udp::socket socket(io_context, endpoint.protocol());
        socket.set_option(boost::asio::ip::udp::socket::reuse_address(true));
        socket.set_option(boost::asio::ip::multicast::join_group(endpoint.address()));
        socket.set_option(boost::asio::socket_base::broadcast(true));
        socket.bind(endpoint);

        try {
            boost::asio::io_service io_service;
            MulticastServer s(io_service, boost::asio::ip::address::from_string(MULTICAST_SERVER_GROUP), end);
            while(!*end) {
                io_service.run();
                io_service.reset();
            }
        } catch (std::exception& e) {
            std::cerr << "Exception: " << e.what() << "\n";
        }
    }

    PiEar::MulticastServer::MulticastServer(boost::asio::io_service& io_service, const boost::asio::ip::address& multicast_address, std::atomic<bool> *kill):
        endpoint_(multicast_address, 6666), socket_(io_service, endpoint_.protocol()), timer_(io_service), message_count_(0), kill_server(kill) {
        handle_timeout(boost::system::error_code());
    }

    void PiEar::MulticastServer::handle_send_to(const boost::system::error_code& error) {
        if (!error && !*kill_server) {
            timer_.expires_from_now(boost::posix_time::seconds(1));
            timer_.async_wait( boost::bind(&MulticastServer::handle_timeout, this, boost::asio::placeholders::error));
//            handle_timeout(boost::system::error_code());
        }
    }

    void PiEar::MulticastServer::handle_timeout(const boost::system::error_code& error) {
        if (!error) {
            std::ostringstream os;
            os << "Message " << message_count_++;
            message_ = os.str();
            socket_.send_to(boost::asio::buffer(compress(&message_)), endpoint_);
            handle_send_to(boost::system::error_code());
        } else {
            std::cerr << "Error: " << error.message() << "\n";
            handle_send_to(boost::system::error_code());
        }
    }

    std::string PiEar::MulticastServer::compress(std::string *stream) {
        boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
        in.push(boost::iostreams::gzip_compressor());
        std::istringstream uncompressed_string(*stream);
        in.push(uncompressed_string);
        std::ostringstream compressed_string;
        boost::iostreams::copy(in, compressed_string);
        return compressed_string.str();
    }
}