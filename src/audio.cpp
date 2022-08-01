#include <iostream>
#include <portaudio.h>
#include <thread>
#include <vector>
#include "audio.h"
#include "channel.h"
#include "logger.h"

namespace PiEar {
    Audio::Audio(std::atomic<bool> *_kill, std::vector<channel*> *channels, int audio_index):
            kill(_kill), channels(channels),
            num_channels(1),
            tmp_buffer((uint16_t*) malloc(sizeof(uint16_t) * BUFFER_CHUNK_SIZE)),
            audio_index(audio_index) {
        set_audio_device(audio_index);
    }
    Audio::~Audio() {
        free(tmp_buffer);
    }
    void Audio::audio_thread() {
        PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::trace) << "Loading audio device";
        const PaDeviceInfo* defaultDeviceInfo = Pa_GetDeviceInfo(audio_index);
        PaStream* stream;
        if (defaultDeviceInfo->maxInputChannels > channels->size()) {
            num_channels = (int) channels->size();
        } else {
            num_channels = defaultDeviceInfo->maxInputChannels;
        }
        source_sample_rate = defaultDeviceInfo->defaultSampleRate;
        for ( auto channel : *channels ) {
            channel->setup_swr_ctx(source_sample_rate);
        }
        PaStreamParameters in_params;
        in_params.device = this->audio_index;
        in_params.channelCount = num_channels;
        in_params.hostApiSpecificStreamInfo = nullptr;
        in_params.sampleFormat = paInt16;
        in_params.suggestedLatency = 0;
        PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::trace) << "Opening audio stream";
        int err = Pa_OpenStream(
                &stream,
                &in_params,
                nullptr,
                source_sample_rate,
                BUFFER_CHUNK_SIZE,
                paClipOff,
                paCallback,
                this);
        if( err != paNoError ) {
            PIEAR_LOG_WITH_FILE_LOCATION(boost::log::trivial::error) << "Error opening audio stream";
            throw std::runtime_error("Error opening audio stream");
        }
        err = Pa_StartStream( stream );
        if( err != paNoError ) {
            PIEAR_LOG_WITH_FILE_LOCATION(boost::log::trivial::error) << "Error starting audio stream";
            throw std::runtime_error("PortAudio error starting stream");
        }
        while(!(*kill)){
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
        err = Pa_CloseStream( stream );
        if( err != paNoError ) {
            PIEAR_LOG_WITH_FILE_LOCATION(boost::log::trivial::error) << "Error closing audio stream";
            throw std::runtime_error("PortAudio error closing stream");
        }
    }
#pragma clang diagnostic push
#pragma ide diagnostic ignored "ConstantFunctionResult"
    auto paCallback( const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData ) -> int {
        (void) outputBuffer;    // Prevent unused variable warnings
        (void) timeInfo;        // Prevent unused variable warnings
        (void) statusFlags;     // Prevent unused variable warnings
        (void) framesPerBuffer; // Prevent unused variable warnings
        return ((Audio*)userData)->classPaCallback((uint16_t *)inputBuffer);
    }
#pragma clang diagnostic pop
    auto Audio::classPaCallback(const uint16_t *inputBuffer) -> int {
        for (int i = 0; i < num_channels; i++) {
            if (!channels->at(i)->enabled) {
                continue;
            }
            for (int j = 0; j < BUFFER_CHUNK_SIZE; j++) {
                tmp_buffer[j] = inputBuffer[num_channels*j+i];
            }
            channels->at(i)->add_sample(tmp_buffer);
        }
        return 0;
    }
// This is because `Pa_Initialize()` needs to be called before anything can be done with PortAudio.
#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-convert-member-functions-to-static"
    auto Audio::channel_count(int audio_index_to_check) -> int {
        return Pa_GetDeviceInfo(audio_index_to_check)->maxInputChannels;
    }
    auto Audio::get_audio_devices() -> std::vector<audioDevice> {
        std::vector<audioDevice> devices;
        int numDevices = Pa_GetDeviceCount();
        for( int i = 0; i < numDevices; i++ ) {
            const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
            audioDevice device;
            device.index = i;
            device.name = deviceInfo->name;
            device.channels = deviceInfo->maxInputChannels;
            devices.push_back(device);
        }
        return devices;
    }
#pragma clang diagnostic pop

    void Audio::set_audio_device(int index) {
        if (this->audio_index < 0 || this->audio_index >= Pa_GetDeviceCount()) {
            this->audio_index = Pa_GetDefaultInputDevice();
        } else {
            this->audio_index = index;
        }
    }
    auto Audio::get_audio_index() const -> int {
        return audio_index;
    }
}
