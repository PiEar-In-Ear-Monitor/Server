#ifndef PIEAR_SERVER_CHANNEL_H
#define PIEAR_SERVER_CHANNEL_H

#include <boost/beast/core/detail/base64.hpp>
#include <iostream>
extern "C" {
    #include <libswresample/swresample.h>
}
#include <string>
#include "ring-buffer.hpp"

#define ADD_KEY_VAL_LIT(s, k, v) (s) << '"' << (k) << "\":" << (v)
#define ADD_KEY_VAL_STR(s, k, v) (s) << '"' << (k) << "\":\"" << (v) << '"'
#define COMMA(s) (s) << ','
#define BUFFER_SIZE 5
#define BUFFER_CHUNK_SIZE 128
#define FINAL_SAMPLE_RATE 44100

namespace PiEar {
    struct return_data {
        uint16_t *data;
        int frames;
        ~return_data() {
            free(data);
        }
    };
    class channel {
    private:
        ringBuffer<uint16_t> buffer;   //!< Ring buffer for the channel
        SwrContext *swr_ctx = nullptr; //!< Swr context for the channel
        int sample_rate = -1;          //!< Sample rate of the channel
        int converted_sample_max = -1;  //!< Maximum number of samples that can be converted
    public:
        /**
         * @param int piear_id
         * @param std::string channel name
         * @param bool enabled (default true)
         * @param buffer_size size of the buffer (default BUFFER_SIZE)
         */
        explicit channel(int pe_id, const std::string& c_name = "", bool en = true, int buffer_size = BUFFER_SIZE) :
                piear_id(pe_id), channel_name(base64_encode(c_name)), buffer(buffer_size * BUFFER_CHUNK_SIZE, BUFFER_CHUNK_SIZE) , enabled(en) {}
        /**
         * @brief Encodes a string into a base64 string
         * @param s String to encode
         * @return Encoded string
         */
        static auto base64_encode(boost::beast::string_view s) -> std::string;
        int piear_id;             //!< ID of channel in piear
        bool enabled;             //!< If the channel is enabled in piear
        std::string channel_name; //!< Channel name
        /**
         * The default `std::string` conversion is to
         * a JSON object
         * @return std::string JSON representation
         */
        explicit operator std::basic_string<char>() const;
        /**
         * The default `bool` conversion is to
         * return if the channel is enabled
         * @return bool `enabled`
         */
        explicit operator bool() const;
        /**
         * @brief Adds a sample to the channel's buffer
         * @param sample The sample to add, just copies, does not free
         */
        auto add_sample(uint16_t *sample) -> void;
        /**
         * @brief Gets the next sample from the channel's buffer, making
         * sure to resample the audio through the SwrContext
         * @return The next sample
         */
        auto get_sample() -> return_data;
        /**
         * @brief Sets up the SwrContext for the channel. It will delete the
         * old SwrContext if it exists, causing loss of audio if not flushed.
         * @param sample_rate The sample rate to convert to
         */
        auto setup_swr_ctx(int sample_rate) -> void;
        /**
         * @brief Returns true if swr_ctx is set up
         * @return bool True if swr_ctx is set up
         */
        auto swr_ctx_init() ->bool;
        /**
         * The default `int` representation is to
         * return the piear_id of the channel
         * @return int `piear_id`
         */
        explicit operator int() const;
        /**
         * Created during testing, compares
         * channel object to in, by comparing
         * piear_id to the other int
         * @return bool `piear_id` == `int`
         */
        auto operator==(int rhs) const -> bool;
    };
}
#endif //PIEAR_SERVER_CHANNEL_H
