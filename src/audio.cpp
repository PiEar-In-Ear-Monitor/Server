#include "audio.h"
#include <pipewire/pipewire.h>
#include <vector>
#include <iostream>

namespace PiEar {
//    std::vector<std::vector<float>> audio_buffer;
//
//    void audio_init() {
//        audio_buffer.resize(PIPEWIRE_CHANNELS);
//        for (auto& channel : audio_buffer) {
//            channel.resize(AUDIO_BUFFER_SIZE);
//        }
//    }
//
//    void audio_get_samples(float* samples, int num_samples) {
//        for (int i = 0; i < num_samples; i++) {
//            for (int channel = 0; channel < PIPEWIRE_CHANNELS; channel++) {
//                samples[i * PIPEWIRE_CHANNELS + channel] = audio_buffer[channel][i];
//            }
//        }
//    }
//
//    void audio_set_samples(const float* samples, int num_samples) {
//        for (int i = 0; i < num_samples; i++) {
//            for (int channel = 0; channel < PIPEWIRE_CHANNELS; channel++) {
//                audio_buffer[channel][i] = samples[i * PIPEWIRE_CHANNELS + channel];
//            }
//        }
//    }

    void audio_thread() {

        pw_init(nullptr, 0);

        fprintf(stdout, "Compiled with libpipewire %s\n"
                        "Linked with libpipewire %s\n",
                pw_get_headers_version(),
                pw_get_library_version());
    }

}