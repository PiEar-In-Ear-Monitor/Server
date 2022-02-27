//******************************************************************************
// Copyright © 2021  Alexander O'Connor
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//******************************************************************************

/**
 * @file http-server.h
 * @brief This is the HTTP server header file for this project.
 * @author Alexander O'Connor
 * @date 30 December 2021
 */

#ifndef PIEAR_SERVER_HTTP_SERVER_H
#define PIEAR_SERVER_HTTP_SERVER_H

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
    void mainloop_http_server(std::atomic<int> *cpm, std::atomic<int> *audio_stream_count, std::atomic<char*> *curr_hash);
}

#endif //PIEAR_SERVER_HTTP_SERVER_H
