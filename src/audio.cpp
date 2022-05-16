#include "audio.h"
#include <portaudio.h>
#include <vector>
#include <iostream>
#include "channel.hpp"

namespace PiEar {
    Audio::Audio(std::atomic<bool> *_kill, std::vector<PiEar::channel*> *channels):
            kill(_kill), channels(channels) {}

    void Audio::audio_thread() {
        int err = Pa_Initialize();
        if( err != paNoError ){
            std::cout << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
            throw std::runtime_error("PortAudio error initializing");
        }
        else std::cout << "PortAudio initialized" << std::endl;
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
        /* Open an audio I/O stream. */
        err = Pa_OpenDefaultStream( &stream,
                                    defaultDeviceInfo->maxInputChannels,  /* Max input */
                                    defaultDeviceInfo->maxInputChannels,  /* Max output */
                                    paInt16,                              /* 16 Bit Audio */
                                    defaultDeviceInfo->defaultSampleRate, /* Best sample rate possible from device TODO: Should this always be so high? */
                                    FRAMES_PER_BUFFER,                    /* frames per buffer TODO: consider: paFramesPerBufferUnspecified */
                                    paCallback,                           /* this is your callback function */
                                    nullptr );                            /* This is a pointer that will be passed to your callback */
        if( err != paNoError ) {
            std::cout << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
            throw std::runtime_error("PortAudio error opening stream");
        }
        else std::cout << "PortAudio stream opened" << std::endl;
        // Start the stream
        err = Pa_StartStream( stream );
        if( err != paNoError ) {
            std::cout << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
            throw std::runtime_error("PortAudio error starting stream");
        }
        else std::cout << "PortAudio stream started" << std::endl;
        // For loop waiting for input
        while(!(*kill)){
            Pa_Sleep(100);
        }
        // Close the stream
        err = Pa_CloseStream( stream );
        if( err != paNoError ) {
            std::cout << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
            throw std::runtime_error("PortAudio error closing stream");
        }
        else std::cout << "PortAudio stream closed" << std::endl;
        // Terminate PortAudio
        Pa_Terminate();
    }

    int Audio::paCallback( const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData ) {
        /* Cast data passed through stream to our structure. */
        paTestData *data = (paTestData*)userData;
        float *out = (float*)outputBuffer;
        (void) inputBuffer; /* Prevent unused variable warning. */
        std::cout << "Size of input buffer: " << sizeof(inputBuffer) << std::endl;
        std::cout << "Size of frames per buffer: " << framesPerBuffer << std::endl;
        for(unsigned int i=0; i<framesPerBuffer; i++ )
        {
            // TODO: add audio processing here
            // Add audio to queue
//            audioQueue.push(out[i]);
        }
        return 0;
    }
}
