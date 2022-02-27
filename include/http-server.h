/**
 * @file http-server.h
 * @brief This is the HTTP server header file for this project.
 * @author Alexander O'Connor
 * @date 30 December 2021
 */

#ifndef PIEAR_SERVER_HTTP_SERVER_H
#define PIEAR_SERVER_HTTP_SERVER_H

#include <string>
#include <atomic>

namespace PiEar {
    /**
     * @brief This runs the webserver.
     * @details On port 8080, this webserver is a getter/setter for the labels and cpm
     * @param cpm pointer to the cpm
     * @param audio_stream_count the number of audio streams
     * @param curr_hash this is the current hash of all labels, so that it is of consistent length.
     * This should be sent with the audio so the app knows if it needs to ask for the latest names.
     */
    void mainloop_http_server(std::atomic<int> *cpm, int audio_stream_count, const std::string& curr_hash);
}

#endif //PIEAR_SERVER_HTTP_SERVER_H
