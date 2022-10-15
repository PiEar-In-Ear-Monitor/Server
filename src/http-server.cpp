#include <atomic>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>
#include <vector>
#include "channel.h"
#include "http-server.h"
#include "logger.h"

namespace PiEar {
    void mainloop_http_server(std::atomic<bool> *kill_switch, std::vector<channel*> *channels, std::atomic<int> *bpm, const std::string& ws_secret, int delay, const std::vector<PiEar::audioDevice>& devices, std::atomic<int> *device_index, std::atomic<bool> *new_audio_device) {
        int server_thread = fork();
        if (!server_thread) {
            std::vector<char*> server_args;
            server_args.push_back(const_cast<char*>("PiEar_HTTP_Server.js"));
            server_args.push_back(const_cast<char*>(ws_secret.c_str()));
            server_args.push_back(nullptr);
            char **command = server_args.data();
            execve((server_executable_dir() + "src/PiEar_HTTP_Server.js").c_str(), &command[0], nullptr);
            return; // Redundant
        }
        sleep(delay);
        std::string host = "localhost";
        std::string port = "9090";
        boost::asio::io_context ioc;
        boost::asio::ip::tcp::resolver resolver{ioc};
        boost::beast::websocket::stream<boost::asio::ip::tcp::socket> ws{ioc};
        boost::asio::connect(ws.next_layer(), resolver.resolve(host, port));
        boost::beast::flat_buffer stream_buffer;
        ws.set_option(boost::beast::websocket::stream_base::decorator(
            [ws_secret](boost::beast::websocket::request_type &req) {
                req.set(boost::beast::http::field::user_agent, "PiEar-Server-1.0");
                req.set("shared-secret", ws_secret);
            }
        ));
        PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::info) << "Connecting to http server...";
        ws.handshake(host + ":" + port, "/");
        for (auto &chan : *channels) {
            ws.write(boost::asio::buffer(std::string(*chan)));
        }
        for (const auto& device: devices) {
            std::stringstream device_string;
            device_string << R"({"device": { "index":)" << device.index << R"(, "channels":)" << device.channels << R"(, "name":")" << device.name << "\"}}";
            ws.write(boost::asio::buffer(device_string.str()));
        }
        std::stringstream final_string;
        final_string << "{\"bpm\":" << *bpm << ", \"active_device\":" << *device_index << "}";
        ws.write(boost::asio::buffer(final_string.str()));
        PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::info) << "Connected to http server";
        std::thread kill_server_thread(kill_server_waiter, server_thread, kill_switch);
        std::string input_string;
        while (!*kill_switch) {
            try {
                stream_buffer.clear();
                ws.read_some(stream_buffer, ws.read_message_max());
                std::ostringstream os;
                os << boost::beast::make_printable(stream_buffer.data());
                input_string = os.str();
                nlohmann::json json_parsed = nlohmann::json::parse(input_string);
                PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::debug) << "Received message from http server: " << json_parsed;
                if (!json_parsed["bpm"].empty()) {
                    *bpm = json_parsed["bpm"];
                } else if (!json_parsed["device"].empty()) {
                    *device_index = json_parsed["device"];
                    *new_audio_device = true;
                } else if (!json_parsed["piear_id"].empty()){
                    for (auto & chan : *channels) {
                        if (chan->piear_id == json_parsed["piear_id"]) {
                            if (!json_parsed["channel_name"].empty()) {
                                chan->channel_name = json_parsed["channel_name"];
                            }
                        }
                    }
                }
            } catch (std::exception const &e) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1100));
                if (!*kill_switch) {
                    PIEAR_LOG_WITH_FILE_LOCATION(boost::log::trivial::error) << "Error reading from HTTP server: " << e.what();
                }
            }
        }
        kill_server_thread.join();
    }
    void kill_server_waiter(int to_kill, std::atomic<bool> *kill_server) {
        while (!*kill_server) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
        kill(to_kill, SIGTERM);
    }
    auto server_executable_dir() -> std::string {
        std::string local_location = "../webserver/";
        std::string remote_location = "/usr/share/piear/webserver/"; // TODO: Make this cross-platform
        if (std::ifstream(local_location).good()) {
            return local_location;
        } else if (std::ifstream(remote_location).good()) {
            return remote_location;
        }
        std::cout << "Could not find server executable" << std::endl;
        exit(1);
    }
}
