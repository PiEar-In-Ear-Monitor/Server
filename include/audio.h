#ifndef PIEAR_SERVER_AUDIO_H
#define PIEAR_SERVER_AUDIO_H

#define FRAMES_PER_BUFFER 128

#include <portaudio.h>
#include <vector>
#include "channel.hpp"
#include <atomic>
#include "queue.hpp"
#include <vector>

namespace PiEar {
    void audio_thread(std::atomic<bool>&, std::vector<PiEar::channel*> *);
    class Audio {
    public:
        queue<std::vector<unsigned short[FRAMES_PER_BUFFER]>> audioQueue;
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
