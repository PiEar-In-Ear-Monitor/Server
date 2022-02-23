#include "click.h"
#include <chrono>
#include <atomic>
#include "globals.h"

namespace PiEar {
    void mainloop_click(std::atomic<int> *cpm) {
        auto start_time = std::chrono::system_clock::now();
        while (!kill_program) {
            click = false;
            while (std::chrono::duration<double>(std::chrono::system_clock::now() - start_time).count() < (60.0 / (double) *cpm));
            click = true;
            start_time = std::chrono::system_clock::now();
        }
    }
}