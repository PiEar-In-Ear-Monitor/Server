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

#ifndef PIEAR_SERVER_MULTICAST_SERVER_H
#define PIEAR_SERVER_MULTICAST_SERVER_H
#define MUTLICAST_SERVER_H_VERSION_MAJOR 1
#define MUTLICAST_SERVER_H_VERSION_MINOR 0
#define MULTICAST_SERVER_PORT 9090
#define MULTICAST_SERVER_GROUP "224.0.0.128"

namespace PiEar {
    /**
     * This will create the multicast server
     *
     * This will continually stream the audio streams
     * @param audio_streams Pointer to all audio streams
     * @param click_stream  Pointer to the click's stream
     * @param end Pointer to a bool that, when set to false, will kill the server
     */
    void mainloop_multicast_server();

    /**
     * Creates the Multicast server
     *
     * @return Pointer to a new Poco::Net::MulticastSocket
     */
    void make_server();

    /**
     * Package data from audio streams
     *
     * Into spec compliant char*
     * @param chars_to_return The end length of all packaged data
     * @param audio_streams The audio streams
     * @param click_stream The click stream
     * @return Spec compliant char*
     */
    unsigned char* package_data(int chars_to_return, int *audio_streams, bool *click_stream);

    /**
     * Handles sending data
     * @param socket Socket to send data on
     * @param data Data to send
     * @return Success
     */
    bool send_data(void *socket, unsigned char *data);
}

#endif //PIEAR_SERVER_MULTICAST_SERVER_H
