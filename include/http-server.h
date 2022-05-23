#ifndef PIEAR_SERVER_HTTP_SERVER_H
#define PIEAR_SERVER_HTTP_SERVER_H

#include <atomic>
#include <string>
#include <vector>
#include "channel.hpp"

namespace PiEar {
    /**
     * @brief This runs the webserver.
     * @details On port 9090, this webserver is a getter/setter for the labels and cpm
     * @param std::atomic<bool> This is the kill swtitch for the http server
     */
    void mainloop_http_server(std::atomic<bool>*, std::vector<channel*>*, std::atomic<int>*, const std::string&, int);
    /**
     * @brief This is used to kill the server thread.
     * @param int This is the pid of the server thread
     * @param std::atomic<bool> This is the kill switch for the http server
     */
    void kill_server_waiter(int, std::atomic<bool>*);
}

#endif //PIEAR_SERVER_HTTP_SERVER_H
