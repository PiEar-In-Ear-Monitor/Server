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

#include <boost/asio/buffer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/udp.hpp>
#include "channel.hpp"

#define MUTLICAST_SERVER_H_VERSION_MAJOR 1
#define MUTLICAST_SERVER_H_VERSION_MINOR 0
#define MULTICAST_SERVER_PORT 6666
#define MULTICAST_SERVER_GROUP "224.0.0.69"

namespace PiEar {
    /**
     * This will create the multicast server
     *
     * This will continually stream the audio streams
     * @param audio_streams Pointer to all audio streams
     * @param click_stream  Pointer to the click's stream
     * @param end Pointer to a bool that, when set to false, will kill the server
     */
    void mainloop_multicast_server(std::vector<channel*>*, std::atomic<bool>*, std::atomic<bool>*);
    /**
     * This is the class for Boost to use
     */
    class MulticastServer {
    public:
        /**
         * This is the constructor for the multicast server
         *
         * @param io_service Service to use
         * @param address IP address to use
         * @param kill Pointer to a bool that, when set to true, will kill the server
         * @param channels Pointer to all channels
         */
        MulticastServer(boost::asio::io_service&, const boost::asio::ip::address&, std::atomic<bool>*, std::atomic<bool>*, std::vector<channel*>*);
        // TODO: Compress the data
//        /**
//         * Compress data with gzip
//         * @param data Data to compress
//         * @return Compressed buffer
//         */
//        static boost::asio::mutable_buffer compress(const uint16_t*);
//        /**
//         * Decompress data with gzip
//         * @param data Data to decompress
//         * @return Decompressed buffer
//         */
//        static boost::asio::mutable_buffer decompress(const uint16_t*);
    private:
        /**
         * This is the function that will be called when the server is started
         * @param ec Error code
         */
        void server_loop(const boost::system::error_code&);
        std::atomic<bool>* kill_server;           //!< Pointer to a bool that, when set to true, will kill the server
        std::atomic<bool>* click;                 //!< Pointer to the click
        boost::asio::ip::udp::endpoint endpoint_; //!< Endpoint to use
        boost::asio::ip::udp::socket socket_;     //!< Socket to use
        std::vector<channel*> *channels;          //!< Pointer to all channels
    };
}

#endif //PIEAR_SERVER_MULTICAST_SERVER_H
