#include "click.h"
#include <chrono>

void mainloop_click(int *cpm, bool *output, bool *end) {
    while(!*end) {
        auto start_time = std::chrono::system_clock::now();
        auto curr_time = std::chrono::system_clock::now();
        while (std::chrono::duration<double>(curr_time - start_time).count() < (60.0 / (double) *cpm)) {
            curr_time = std::chrono::system_clock::now();
            *output = false;
        }
        *output = true;
    }
}