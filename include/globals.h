#ifndef PIEAR_SERVER_GLOBALS_H
#define PIEAR_SERVER_GLOBALS_H

namespace PiEar {
    extern std::vector<channel> channels;
    extern std::mutex channel_mtx;
    extern bool click;
    extern bool kill_program;
}


#endif //PIEAR_SERVER_GLOBALS_H
