#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sys/poll.h>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

// Sends a WebSocket message and prints the response
int main(int argc, char** argv)
{
    try
    {
        // Check command line arguments.
        if(argc != 3)
        {
            std::cerr <<
                "Usage: websocket-client-sync <host> <port>\n" <<
                "Example:\n" <<
                "    websocket-client-sync echo.websocket.org 80\n";
            return EXIT_FAILURE;
        }
        std::string host = argv[1];
        auto const  port = argv[2];

        // The io_context is required for all I/O
        net::io_context ioc;

        // These objects perform our I/O
        tcp::resolver resolver{ioc};
        websocket::stream<tcp::socket> ws{ioc};

        // Update the host_ string. This will provide the value of the
        // Host HTTP header during the WebSocket handshake.
        // See https://tools.ietf.org/html/rfc7230#section-5.4
        host += ':' + std::to_string(net::connect(ws.next_layer(), resolver.resolve(host, port)).port());

        // Set a decorator to change the User-Agent of the handshake
        ws.set_option(websocket::stream_base::decorator(
            [](websocket::request_type& req)
            {
                req.set(http::field::user_agent, "PiEar-Server-1.0");
                req.set("Shared-Secret", "Hello-World");
            }));

        // Perform the websocket handshake
        ws.handshake(host, "/"); // TODO Consider making custom URL

        // This buffer will hold the incoming message
        beast::flat_buffer buffer;
        
        // Send the message
        ws.write(net::buffer("{\"piear_id\":0,\"channel_name\":\"Bass\"}"));
        ws.write(net::buffer("{\"piear_id\":1,\"channel_name\":\"Cajon\"}"));
        ws.write(net::buffer("{\"piear_id\":2,\"channel_name\":\"Desther\"}"));
        ws.write(net::buffer("{\"piear_id\":3,\"channel_name\":\"Keys\"}"));
        ws.write(net::buffer("{\"piear_id\":4,\"channel_name\":\"Guitar\"}"));
        ws.write(net::buffer("{\"BPM\":150}"));

        struct pollfd fds;
        fds.fd = 0; /* this is STDIN */
        fds.events = POLLIN;
        while (!poll(&fds, 1, 0)) {
            ws.read(buffer);
            std::cout << "Received " << beast::make_printable(buffer.data()) << std::endl;
            buffer.clear();
        }
        ws.write(net::buffer("kill"));
        // Close the WebSocket connection
        ws.close(websocket::close_code::normal);

        // If we get here then the connection is closed gracefully

        // The make_printable() function helps print a ConstBufferSequence
        std::cout << beast::make_printable(buffer.data()) << std::endl;
    }
    catch(std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}