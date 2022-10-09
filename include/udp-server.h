#ifndef PIEAR_SERVER_UDP_SERVER_H
#define PIEAR_SERVER_UDP_SERVER_H

#define UDP_SERVER_H_VERSION_MAJOR 1
#define UDP_SERVER_H_VERSION_MINOR 0
#define UDP_SERVER_PORT 6666
#define UDP_SERVER_GROUP "224.0.0.69"

#include <boost/asio/buffer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/udp.hpp>
#include <mutex>
#include "channel.h"

namespace PiEar {
    /**
     * This will create the multicast server
     *
     * This will continually stream the audio streams
     * @param audio_streams Pointer to all audio streams
     * @param click_stream  Pointer to the click's stream
     * @param end Pointer to a bool that, when set to false, will kill the server
     */
    void mainloop_udp_server(std::vector<channel*>*, std::atomic<bool>*, std::atomic<bool>*);
    /**
     * This is the class for Boost to use
     */
    class UdpServer {
    public:
        /**
         * This is the constructor for the udp server
         *
         * @param io_context Context to use
         * @param kill Pointer to a bool that, when set to true, will kill the server
         * @param channels Pointer to all channels
         */
        UdpServer(boost::asio::io_context&, std::atomic<bool>*, std::atomic<bool>*, std::vector<channel*>*);
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
         */
        void server_loop();
        /**
         * This is the function sends the click stream
         * @param ec Error code
         */
        void send_click_stream();
        std::atomic<bool>* kill_server;           //!< Pointer to a bool that, when set to true, will kill the server
        std::atomic<bool>* click;                 //!< Pointer to the click
        boost::asio::ip::udp::endpoint endpoint_; //!< Endpoint to use
        boost::asio::ip::udp::socket socket_;     //!< Socket to use
        std::vector<channel*> *channels;          //!< Pointer to all channels
        std::mutex mutex_;                        //!< Mutex to use
    };
}
#endif //PIEAR_SERVER_UDP_SERVER_H
