#include "audio.h"
#include <portaudio.h>
#include <vector>
#include <iostream>

namespace PiEar {
    
    void audio_thread() {
        int err = Pa_Initialize();
        if( err != paNoError ) std::cout << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
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
        err = Pa_OpenDefaultStream( &stream, 0, 1, paFloat32, 44100, 1024, NULL, NULL );
        if( err != paNoError ) std::cout << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        else std::cout << "PortAudio stream opened" << std::endl;
        // Start the stream
        err = Pa_StartStream( stream );
        if( err != paNoError ) std::cout << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        else std::cout << "PortAudio stream started" << std::endl;
        // For loop waiting for input
        while( true ) {
            // Wait for input
            std::string input;
            std::cin >> input;
            if (input == "quit") break;
        }
        if( err != paNoError ) std::cout << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        else std::cout << "PortAudio stream finished" << std::endl;
        // Close the stream
        err = Pa_CloseStream( stream );
        if( err != paNoError ) std::cout << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        else std::cout << "PortAudio stream closed" << std::endl;
        // Terminate PortAudio
        Pa_Terminate();
    }
}