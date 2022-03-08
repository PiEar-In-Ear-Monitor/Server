/**
 * @file http-server.h
 * @brief This is the HTTP server header file for this project.
 * @author Alexander O'Connor
 * @date 30 December 2021
 */

#ifndef PIEAR_SERVER_HTTP_SERVER_H
#define PIEAR_SERVER_HTTP_SERVER_H

#include "channel.hpp"
#include <string>
#include <atomic>
#include <thread>
#include <vector>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace PiEar {
    /**
     * @brief This runs the webserver.
     * @details On port 9090, this webserver is a getter/setter for the labels and cpm
     * @param std::atomic<bool> This is the kill swtitch for the http server
     */
    void mainloop_http_server(std::atomic<bool>*, std::vector<channel*>*, std::atomic<int>*);

    void kill_server_waiter(int, std::atomic<bool>*);
}

#endif //PIEAR_SERVER_HTTP_SERVER_H
