#ifndef PIEAR_SERVER_AUDIO_H
#define PIEAR_SERVER_AUDIO_H

#include <atomic>
#include <portaudio.h>
#include <vector>
#include "channel.h"

namespace PiEar {
    /**
     * @brief This routine is the PortAudio callback
     */
    auto paCallback( const void*, void*, unsigned long, const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void* ) -> int;
    /**
     * @brief This struct is used to get data for choosing a device.
     */
    struct audioDevice {
        std::string name;
        int index;
        int channels;
    };
    /**
     * @brief The Audio class is responsible for handling the audio input and placing it into the queue.
     * It uses PortAudio to handle the audio input and output.
     * It uses a queue to store the audio data.
     */
    class Audio {
    public:
        /**
         * @brief Audio Constructor
         * @param kill The kill flag.
         * @param channels The channels where the audio will be stored.
         * @param audio_index The index of the audio channel to use.
         */
        Audio(std::atomic<bool>*, std::vector<channel*>*, int);
        /**
         * @brief Audio Destructor
         */
        ~Audio();
        /**
         * @brief audio thead to handle the audio input and output.
         */
        void audio_thread();
        /**
         * @brief returns the number of channels provided by a source.
         * @param
         * @return The number of channels.
         */
        auto channel_count(int) -> int;
        /**
         * @brief This is the class's PA handler.
         */
        auto classPaCallback( const uint16_t*) -> int;
        /**
         * @brief This function returns a std::vector of all channel names, indexes, and input channels
         * @return std::vector<std::string> A vector of channel names, indexes, and input channels.
         */
        auto get_audio_devices() -> std::vector<audioDevice>;
        /**
         * @brief This function returns the index of the audio device to use.
         * @return int The index of the audio device to use.
         */
        void set_audio_device(int);
        /**
         * @brief This function returns the current audio device index.
         * @return int The current audio device index.
         */
        [[nodiscard]] auto get_audio_index() const -> int;
    private:
        int audio_index;                 //!< The index of the audio channel to use.
        std::vector<channel*>* channels; //!< The channels where the audio will be stored.
        std::atomic<bool> *kill;         //!< The kill flag.
        int num_channels;                //!< The number of channels.
        uint16_t *tmp_buffer;            //!< The temporary buffer.
        int source_sample_rate = -1;     //!< The source sample rate.
    };
}
#endif //PIEAR_SERVER_AUDIO_H
