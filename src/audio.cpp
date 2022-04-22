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
        int numDevices = Pa_GetDeviceCount();
        std::cout << "Number of devices: " << numDevices << std::endl;
        // Print out the names of the devices
        for( int i = 0; i < numDevices; i++ ) {
            const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
            std::cout << "Device " << i << ": " << deviceInfo->name << std::endl;
        }
        // Open the default device
        PaStream* stream;
        /* Open an audio I/O stream. */
        err = Pa_OpenDefaultStream( &stream,
                                    64,          /* input channels */
                                    0,          /* stereo output */
                                    paFloat32,  /* 32 bit floating point output */
                                    44100,
                                    256,        /* frames per buffer, i.e. the number
                                                   of sample frames that PortAudio will
                                                   request from the callback. Many apps
                                                   may want to use
                                                   paFramesPerBufferUnspecified, which
                                                   tells PortAudio to pick the best,
                                                   possibly changing, buffer size.*/
                                    paCallback, /* this is your callback function */
                                    nullptr ); /*This is a pointer that will be passed to your callback*/
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

    int Audio::paCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData ) {
        /* Cast data passed through stream to our structure. */
        paTestData *data = (paTestData*)userData;
        float *out = (float*)outputBuffer;
        (void) inputBuffer; /* Prevent unused variable warning. */
        for(unsigned int i=0; i<framesPerBuffer; i++ )
        {
            // TODO: add audio processing here
        }
        return 0;
    }
}