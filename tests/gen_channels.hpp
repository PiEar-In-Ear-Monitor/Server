#ifndef PIEAR_SERVER_GEN_CHANNELS_HPP
#define PIEAR_SERVER_GEN_CHANNELS_HPP

#include <random>
#include <vector>
#include "channel.hpp"

void generate_channels(int number_of_channels, std::vector<PiEar::channel*> *current_vector) {
    int channel_count = current_vector->size();
    for (int i = 0; i < number_of_channels; ++i) {
        current_vector->push_back(new PiEar::channel(channel_count, std::string("Channel " + std::to_string(channel_count))));
        channel_count++;
    }
}
std::vector<PiEar::channel*>* generate_channels(int number_of_channels) {
    auto vector = new std::vector<PiEar::channel*>;
    generate_channels(number_of_channels, vector);
    return vector;
}
#endif //PIEAR_SERVER_GEN_CHANNELS_HPP
