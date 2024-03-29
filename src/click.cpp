#include <atomic>
#include <chrono>
#include <thread>
#include "click.h"

namespace PiEar {
    void mainloop_click(std::atomic<int> *cpm, std::atomic<bool> *click, std::atomic<bool> *kill_click) {
        auto start_time = std::chrono::system_clock::now();
        while (!*kill_click) {
            *click = false;
            while (std::chrono::duration<double>(std::chrono::system_clock::now() - start_time).count() < (60.0 / (double) *cpm)) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            *click = true;
            start_time = std::chrono::system_clock::now();
        }
    }
}