#ifndef PIEAR_SERVER_AUDIO_H
#define PIEAR_SERVER_AUDIO_H

#define FRAMES_PER_BUFFER 128

#include <atomic>
#include <portaudio.h>
#include <vector>
#include "channel.hpp"

// TODO: Expose the number of channels
namespace PiEar {
    /**
     * @brief This routine is the PortAudio callback
     */
    int paCallback( const void*, void*, unsigned long, const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void* );
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
         */
        Audio(std::atomic<bool>*, std::vector<channel*>*);
        /**
         * @brief Audio Destructor
         */
        ~Audio();
        /**
         * @brief audio thead to handle the audio input and output.
         */
        void audio_thread();
        /**
         * @brief returns the number of channels.
         * @return The number of channels.
         */
        [[nodiscard]] int channel_count() const;
        /**
         * @brief This is the class's PA handler.
         */
        int classPaCallback( const uint16_t*);
    private:
        std::vector<channel*>* channels; //!< The channels where the audio will be stored.
        std::atomic<bool> *kill; //!< The kill flag.
        int num_channels; //!< The number of channels.
        uint16_t *tmp_buffer; //!< The temporary buffer.
    };
}

#endif //PIEAR_SERVER_AUDIO_H
