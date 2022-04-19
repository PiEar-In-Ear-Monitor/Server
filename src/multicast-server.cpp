#include "multicast-server.h"
#include "channel.hpp"
#include <atomic>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace PiEar {
    void mainloop_multicast_server(std::vector<channel*> *audio_streams, std::atomic<bool> *click_stream, std::atomic<bool> *end)
    {
        boost::asio::io_context io_context;
        boost::asio::ip::udp::endpoint endpoint(boost::asio::ip::address::from_string(MULTICAST_SERVER_GROUP), MULTICAST_SERVER_PORT);
        boost::asio::ip::udp::socket socket(io_context, endpoint.protocol());
        socket.set_option(boost::asio::ip::udp::socket::reuse_address(true));
        socket.set_option(boost::asio::ip::multicast::join_group(endpoint.address()));
//        socket.set_option(boost::asio::ip::multicast::enable_loopback(false));
//        socket.set_option(boost::asio::ip::multicast::hops(1));
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
        std::ostringstream os;
        os << "Message " << message_count_++;
        message_ = os.str();
        socket_.async_send_to( boost::asio::buffer(message_), endpoint_, boost::bind(&MulticastServer::handle_send_to, this,boost::asio::placeholders::error));
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
            socket_.async_send_to( boost::asio::buffer(message_), endpoint_, boost::bind(&MulticastServer::handle_send_to, this, boost::asio::placeholders::error));
        }
    }
}