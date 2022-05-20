//******************************************************************************
// Copyright © 2022  Alexander O'Connor
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

//
// Created by alex on 3/4/22.
//

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
