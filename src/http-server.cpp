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
    void mainloop_http_server(std::atomic<bool> *kill_switch, std::vector<channel*> *channels, std::atomic<int> *bpm, const std::string& ws_secret, int delay) {
        // Start server thread
        int server_thread = fork();
        if (!server_thread) {
            std::vector<char*> server_args;
            server_args.push_back(const_cast<char*>("PiEar_HTTP_Server.js"));
            server_args.push_back(const_cast<char*>(ws_secret.c_str()));
            server_args.push_back(nullptr);
            char **command = server_args.data();
            execve("../PiEar_HTTP_Server/src/PiEar_HTTP_Server.js", &command[0], nullptr);
            return; // Redundant
        }
        sleep(delay);
        std::string host = "localhost";
        std::string port = "9090";
        boost::asio::io_context ioc;
        boost::asio::ip::tcp::resolver resolver{ioc};
        boost::beast::websocket::stream<boost::asio::ip::tcp::socket> ws{ioc};
        // Connect to server thread
        host += ':' + std::to_string(boost::asio::connect(ws.next_layer(), resolver.resolve(host, port)).port());
        boost::beast::flat_buffer stream_buffer;


        // Set a decorator to change the User-Agent of the handshake
        ws.set_option(boost::beast::websocket::stream_base::decorator(
            [ws_secret](boost::beast::websocket::request_type &req) {
                    req.set(boost::beast::http::field::user_agent, "PiEar-Server-1.0");
                    req.set("shared-secret", ws_secret);
                }
            ));

        ws.handshake(host, "/");

        for (auto &chan : *channels) {
            ws.write(boost::asio::buffer(std::string(*chan)));
        }
        std::stringstream bpm_string;
        bpm_string << "{\"BPM\":" << *bpm << "}";
        ws.write(boost::asio::buffer(bpm_string.str()));

        std::thread kill_server_thread(kill_server_waiter, server_thread, kill_switch);
        while (!*kill_switch) {
            try {
                ws.read(stream_buffer);
            } catch(std::exception const& e) {}
            try {
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
            } catch (std::exception const &e) {}
        }
        kill_server_thread.join();
    }

    void kill_server_waiter(int to_kill, std::atomic<bool> *kill_server) {
        while (!*kill_server) std::this_thread::yield();
        kill(to_kill, SIGTERM);
    }
}
