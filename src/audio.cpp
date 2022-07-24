#include <iostream>
#include <portaudio.h>
#include <vector>
#include "audio.h"
#include "channel.hpp"

namespace PiEar {
    Audio::Audio(std::atomic<bool> *_kill, std::vector<channel*> *channels, int audio_index):
            kill(_kill), channels(channels),
            num_channels(1),
            tmp_buffer((uint16_t*) malloc(sizeof(uint16_t) * FRAMES_PER_BUFFER)),
            audio_index(audio_index) {
        if (this->audio_index < 0 || this->audio_index >= Pa_GetDeviceCount()) {
            this->audio_index = Pa_GetDefaultInputDevice();
        }
    }
    Audio::~Audio() {
        free(tmp_buffer);
    }
    void Audio::audio_thread() {
        // Find out how many devices there are
        const PaDeviceInfo* defaultDeviceInfo = Pa_GetDeviceInfo(audio_index);
        PaStream* stream;
        if (defaultDeviceInfo->maxInputChannels > channels->size()) {
            num_channels = (int) channels->size();
        } else {
            num_channels = defaultDeviceInfo->maxInputChannels;
        }
        PaStreamParameters in_params;
        in_params.device = this->audio_index;
        in_params.channelCount = num_channels;
        in_params.hostApiSpecificStreamInfo = nullptr;
        in_params.sampleFormat = paInt16;
        in_params.suggestedLatency = 0;
        int err = Pa_OpenStream(
                &stream,
                &in_params,
                nullptr,
                defaultDeviceInfo->defaultSampleRate,
                FRAMES_PER_BUFFER,
                paClipOff,
                paCallback,
                this);
        if( err != paNoError ) {
            // TODO: Log this error
            throw std::runtime_error("PortAudio error opening stream");
        }
        err = Pa_StartStream( stream );
        if( err != paNoError ) {
            // TODO: Log this error
            throw std::runtime_error("PortAudio error starting stream");
        }
        while(!(*kill)){
            Pa_Sleep(100);
        }
        err = Pa_CloseStream( stream );
        if( err != paNoError ) {
            // TODO: Log this error
            throw std::runtime_error("PortAudio error closing stream");
        }
    }
#pragma clang diagnostic push
#pragma ide diagnostic ignored "ConstantFunctionResult"
    int paCallback( const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData ) {
        (void) outputBuffer; // Prevent unused variable warnings
        (void) timeInfo; // Prevent unused variable warnings
        (void) statusFlags; // Prevent unused variable warnings
        (void) framesPerBuffer ; // Prevent unused variable warnings
        return ((Audio*)userData)->classPaCallback((uint16_t *)inputBuffer);
    }
#pragma clang diagnostic pop
    int Audio::classPaCallback(const uint16_t *inputBuffer) {
        for (int i = 0; i < num_channels; i++) {
            if (!channels->at(i)->enabled) {
                continue;
            }
            for (int j = 0; j < FRAMES_PER_BUFFER; j++) {
                tmp_buffer[j] = inputBuffer[num_channels*j+i];
            }
            channels->at(i)->buffer.push(tmp_buffer);
        }
        return 0;
    }
// This is because `Pa_Initialize()` needs to be called before anything can be done with PortAudio.
#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-convert-member-functions-to-static"
    int Audio::channel_count(int audio_index_to_check) {
        return Pa_GetDeviceInfo(audio_index_to_check)->maxInputChannels;
    }
    std::vector<audioDevice> Audio::get_audio_devices() {
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
}
