#include <array>
#include <cmath>
#include <gtest/gtest.h>
#include <thread>
#include "click.h"

#define MARGIN_OF_ERROR 0.01

enum {
    SLEEP_TIME =      2
};

void counter(std::atomic<int> *click_count, std::atomic<bool> *click, std::atomic<bool> *end_count) {
    while (!*end_count) {
        if (*click) {
            (*click_count)++;
            while (*click);
        }
    }
}
auto run_test(std::atomic<bool> *click) -> int {
    std::atomic<bool> end_count = false;
    std::atomic<int> click_count = 0;
    std::thread count(counter, &click_count, click, &end_count);
    sleep(SLEEP_TIME);
    end_count = true;
    count.join();
    return click_count;
}
TEST(testPiEar, test_click) {
    std::atomic<int> cpm = 0;
    std::atomic<bool> click = false, end_click = false;
    std::thread click_thread(PiEar::mainloop_click, &cpm, &click, &end_click);
    std::array<int, 6> cpm_values = {999, 500, 300, 150, 120, 60};
    for (auto item : cpm_values){
        cpm = item;
        EXPECT_NEAR(((double)cpm / (60.0 / (double)SLEEP_TIME)), run_test(&click), ceil(((double)cpm*MARGIN_OF_ERROR) + 0.5));
    }
    end_click = true;
    click_thread.join();
    EXPECT_TRUE(click);
}
