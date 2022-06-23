#include <iostream>
#include <portaudio.h>
#include <vector>
#include "audio.h"
#include "channel.hpp"

namespace PiEar {
    Audio::Audio(std::atomic<bool> *_kill, std::vector<channel*> *channels):
            kill(_kill), channels(channels),
            num_channels(-1),
            tmp_buffer((uint16_t*) malloc(sizeof(uint16_t) * FRAMES_PER_BUFFER)) {}
    Audio::~Audio() {
        free(tmp_buffer);
    }
    void Audio::audio_thread() {
        int err = Pa_Initialize();
        if( err != paNoError ){
            // TODO: Log this error
            throw std::runtime_error("PortAudio error initializing");
        }
        // Find out how many devices there are
        const PaDeviceInfo* defaultDeviceInfo = Pa_GetDeviceInfo(Pa_GetDefaultOutputDevice()); // TODO: Make this configurable
//        int numDevices = Pa_GetDeviceCount();
//        std::cout << "Number of devices: " << numDevices << std::endl;
//        // Print out the names of the devices
//        for( int i = 0; i < numDevices; i++ ) {
//            const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
//            std::cout << "Device " << i << ": " << deviceInfo->name << std::endl;
//            std::cout << "\tMax Input Channels: " << deviceInfo->maxInputChannels << std::endl;
//            std::cout << "\tMax Output Channels: " << deviceInfo->maxOutputChannels << std::endl;
//            std::cout << "\tDefault Sample Rate: " << deviceInfo->defaultSampleRate << std::endl;
//        }
        // Open the default device
        PaStream* stream;
        if (defaultDeviceInfo->maxInputChannels > channels->size()) {
            num_channels = (int) channels->size();
        } else {
            num_channels = defaultDeviceInfo->maxInputChannels;
        }
        err = Pa_OpenDefaultStream( &stream,
                                    num_channels,  /* Max input */
                                    0, /* Max output */
                                    paInt16,                              /* 16 Bit Audio */
                                    defaultDeviceInfo->defaultSampleRate, /* Best sample rate possible from device TODO: Should this always be so high? */
                                    FRAMES_PER_BUFFER,                    /* frames per buffer TODO: consider: paFramesPerBufferUnspecified */
                                    paCallback,                           /* this is your callback function */
                                    this );                            /* This is a pointer that will be passed to your callback */
        if( err != paNoError ) {
            // TODO: Log this error
            throw std::runtime_error("PortAudio error opening stream");
        }
        err = Pa_StartStream( stream );
        if( err != paNoError ) {
            // TODO: Log this error
            throw std::runtime_error("PortAudio error starting stream");
        }
        num_channels = defaultDeviceInfo->maxOutputChannels;
        while(!(*kill)){
            Pa_Sleep(100);
        }
        err = Pa_CloseStream( stream );
        if( err != paNoError ) {
            // TODO: Log this error
            throw std::runtime_error("PortAudio error closing stream");
        }
        Pa_Terminate();
    }
    int paCallback( const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData ) {
        (void) outputBuffer; // Prevent unused variable warnings
        (void) timeInfo; // Prevent unused variable warnings
        (void) statusFlags; // Prevent unused variable warnings
        (void) framesPerBuffer ; // Prevent unused variable warnings
        return ((Audio*)userData)->classPaCallback((uint16_t *)inputBuffer);
    }
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
    int Audio::channel_count() const {
        int err = Pa_Initialize();
        if( err != paNoError ){
            // TODO: Log this error
            throw std::runtime_error("PortAudio error initializing");
        }
        const PaDeviceInfo* defaultDeviceInfo = Pa_GetDeviceInfo(Pa_GetDefaultOutputDevice()); // TODO: This will need to be configurable
        return defaultDeviceInfo->maxInputChannels;
    }
}
