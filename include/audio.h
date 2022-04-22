//******************************************************************************
// Copyright © 2021  Alexander O'Connor
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//******************************************************************************

#ifndef PIEAR_SERVER_AUDIO_H
#define PIEAR_SERVER_AUDIO_H

#include <portaudio.h>
#include <vector>
#include "channel.hpp"
#include <atomic>

namespace PiEar {
    void audio_thread(std::atomic<bool>&, std::vector<PiEar::channel*> *);
    class Audio {
    public:
        Audio(std::atomic<bool>*, std::vector<PiEar::channel*> *);
        void audio_thread();
    private:
        typedef struct
        {
            float left_phase;
            float right_phase;
        }
        paTestData;
        std::atomic<bool> *kill;
        std::vector<PiEar::channel*> *channels;
        /* This routine will be called by the PortAudio engine when audio is needed.
        * It may called at interrupt level on some machines so don't do anything
        * that could mess up the system like calling malloc() or free().
        */
        static int paCallback( const void*, void*, unsigned long, const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void* );
    };
}

#endif //PIEAR_SERVER_AUDIO_H
