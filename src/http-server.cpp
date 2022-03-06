#include "http-server.h"
#include "channel.hpp"
#include <vector>
#include <atomic>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>

namespace PiEar {
    void mainloop_http_server(std::atomic<bool> *kill_switch, std::vector<channel*> *channels, std::atomic<int> *bpm) {
        // Start server thread
        int server_thread = fork();
        if (!server_thread) {
            char * const command[] = {"index.js", "kill", "Hello-World", nullptr}; // TODO Randomize Endpoints
            execve("/home/alex/Documents/Uni-Private/2022/Winter/CPTR-488/Server/PiEar_HTTP_Server/index.js", command, nullptr);
            return; // Redundant
        }
        sleep(1);
        std::string host = "localhost";
        std::string port = "9090";
        boost::asio::io_context ioc;
        boost::asio::ip::tcp::resolver resolver{ioc};
        boost::beast::websocket::stream<boost::asio::ip::tcp::socket> ws{ioc};
        // Connect to server thread
        host += ':' + std::to_string(boost::asio::connect(ws.next_layer(), resolver.resolve(host, port)).port());

        try {

            // Set a decorator to change the User-Agent of the handshake
            ws.set_option(boost::beast::websocket::stream_base::decorator(
                [](boost::beast::websocket::request_type &req) {
                        req.set(boost::beast::http::field::user_agent, "PiEar-Server-1.0");
                        req.set("Shared-Secret", "Hello-World");
                    }
                ));

            ws.handshake(host, "/");

            for (auto &chan : *channels) {
                ws.write(boost::asio::buffer(std::string(*chan)));
            }
            std::stringstream bpm_string;
            bpm_string << "{\"BPM\":" << *bpm << "}";
            ws.write(boost::asio::buffer(bpm_string.str()));
            int kill_thread = fork();
            if (!kill_thread) {
                boost::beast::flat_buffer stream_buffer;
                while (true) {
                    ws.read(stream_buffer);
                    std::cout << "Received " << boost::beast::make_printable(stream_buffer.data()) << std::endl;
                    std::ostringstream os;
                    os << boost::beast::make_printable(stream_buffer.data());
                    std::string input_string = os.str();
                    nlohmann::json json_parsed = nlohmann::json::parse(input_string);
                    stream_buffer.clear();
                    if ((json_parsed)["piear_id"].empty()) {
                        if (!(json_parsed)["bpm"].empty()) {
                            *bpm =  (json_parsed)["bpm"];
                        }
                    } else {
                        if ((json_parsed)["piear_id"].empty()) {
                            continue;
                        }
                        for (auto & chan : *channels) {
                            if (chan->piear_id == (json_parsed)["piear_id"]) {
                                if (!(json_parsed)["channel_name"].empty()) {
                                    chan->channel_name = (json_parsed)["channel_name"];
                                }
                            }
                        }
                    }
                }
            }
            do pthread_yield(); while (!*(kill_switch));

            kill(server_thread, SIGTERM);
            kill(kill_thread, SIGTERM);

            // Close the WebSocket connection
        } catch(std::exception const& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}
