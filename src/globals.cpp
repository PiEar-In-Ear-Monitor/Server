#include "channel.h"
#include <vector>
#include <mutex>

namespace PiEar {
    std::vector<channel> channels;
    std::mutex channel_mtx;

    bool click = false; // Only change in click
    bool kill_program = false; // Only change in PiEar
}