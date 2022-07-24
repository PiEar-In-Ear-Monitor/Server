#ifndef PIEAR_SERVER_HTTP_SERVER_H
#define PIEAR_SERVER_HTTP_SERVER_H

#include <atomic>
#include <string>
#include <vector>
#include "audio.h"
#include "channel.hpp"

namespace PiEar {
    /**
     * @brief This runs the webserver.
     * @details On port 9090, this webserver is a getter/setter for the labels and cpm
     * @param std::atomic<bool> This is the kill swtitch for the http server
     * @param std::vector<PiEar::channel*> This is the vector of channels to use.
     * @param std::atomic<int> This is the BPM.
     * @param const std::string This is the secret for the websocket.
     * @param int This is the delay after starting the server to wait before initializing it.
     * @param std::vector<PiEar::audioDevice> This is the vector of audio devices to use.
     * @param std::atomic<int> This is the index of the audio device to use.
     * @param std::atomic<bool> This is an indicator of a changed audio index
     */
    void mainloop_http_server(std::atomic<bool>*, std::vector<channel*>*, std::atomic<int>*, const std::string&, int, const std::vector<audioDevice>&, std::atomic<int>*, std::atomic<bool>*);
    /**
     * @brief This is used to kill the server thread.
     * @param int This is the pid of the server thread
     * @param std::atomic<bool> This is the kill switch for the http server
     */
    void kill_server_waiter(int, std::atomic<bool>*);
    /**
     * @brief This is used to get the webserver's dir.
     * @return std::string This is the webserver's dir.
     */
    std::string server_executable_dir();
}

#endif //PIEAR_SERVER_HTTP_SERVER_H
