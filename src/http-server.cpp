//#include "http-server.h"
#include <string>
#include <atomic>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sys/poll.h>

namespace PiEar {
    void mainloop_http_server(std::atomic<int> *cpm, int audio_stream_count, const std::string& curr_hash) {
        try
        {
            // Check command line arguments.
            std::string host = "localhost";
            auto const  port = "9090";

            // The io_context is required for all I/O
            boost::asio::io_context ioc;

            // These objects perform our I/O
            boost::asio::ip::tcp::resolver resolver{ioc};
            boost::beast::websocket::stream<boost::asio::ip::tcp::socket> ws{ioc};

            // Update the host_ string. This will provide the value of the
            // Host HTTP header during the WebSocket handshake.
            // See https://tools.ietf.org/html/rfc7230#section-5.4
            host += ':' + std::to_string(boost::asio::connect(ws.next_layer(), resolver.resolve(host, port)).port());

            // Set a decorator to change the User-Agent of the handshake
            ws.set_option(boost::beast::websocket::stream_base::decorator(
                    [](boost::beast::websocket::request_type& req)
                    {
                        req.set(boost::beast::http::field::user_agent, "PiEar-Server-1.0");
                        req.set("Shared-Secret", "Hello-World");
                    }));

            // Perform the websocket handshake
            ws.handshake(host, "/"); // TODO Consider making custom URL

            // This buffer will hold the incoming message
            boost::beast::flat_buffer buffer;

            // Send the message
            ws.write(boost::asio::buffer(R"({"piear_id":0,"channel_name":"Bass"})"));
            ws.write(boost::asio::buffer(R"({"piear_id":1,"channel_name":"Cajon"})"));
            ws.write(boost::asio::buffer(R"({"piear_id":2,"channel_name":"Desther"})"));
            ws.write(boost::asio::buffer(R"({"piear_id":3,"channel_name":"Keys"})"));
            ws.write(boost::asio::buffer(R"({"piear_id":4,"channel_name":"Guitar"})"));
            ws.write(boost::asio::buffer(R"({"BPM":150})"));

            struct pollfd fds{};
            fds.fd = 0; /* this is STDIN */
            fds.events = POLLIN;
            while (!poll(&fds, 1, 0)) {
                ws.read(buffer);
                std::cout << "Received " << boost::beast::make_printable(buffer.data()) << std::endl;
                buffer.clear();
            }
            ws.write(boost::asio::buffer("kill"));
            // Close the WebSocket connection
            ws.close(boost::beast::websocket::close_code::normal);

            // If we get here then the connection is closed gracefully

            // The make_printable() function helps print a ConstBufferSequence
            std::cout << boost::beast::make_printable(buffer.data()) << std::endl;
        }
        catch(std::exception const& e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}

int main() {
    PiEar::mainloop_http_server(new std::atomic<int>(100), 4, "Hello-World");
}