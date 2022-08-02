#ifndef PIEAR_SERVER_GEN_CHANNELS_HPP
#define PIEAR_SERVER_GEN_CHANNELS_HPP

#include <random>
#include <vector>
#include "channel.h"

void generate_channels(int number_of_channels, std::vector<PiEar::channel*> *current_vector) {
    for (int i = 0; i < number_of_channels; ++i) {
        current_vector->push_back(new PiEar::channel(current_vector->size(), std::string("Channel " + std::to_string(current_vector->size()))));
    }
}
auto generate_channels(int number_of_channels) -> std::vector<PiEar::channel*>* {
    auto vector = new std::vector<PiEar::channel*>;
    generate_channels(number_of_channels, vector);
    return vector;
}
#endif //PIEAR_SERVER_GEN_CHANNELS_HPP
