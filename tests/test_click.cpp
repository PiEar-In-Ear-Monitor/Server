#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

#include "gtest/gtest.h"
#include "click.h"
#include "globals.h"
#include <thread>
#include <array>

#define SLEEP_TIME      2
#define MARGIN_OF_ERROR 0.01

void counter(std::atomic<int> *count, std::atomic<bool> *end) {
    while (!*end) {
        if (PiEar::click) {
            (*count)++;
            while (PiEar::click);
        }
    }
}

int run_test() {
    std::atomic<bool> end_count = false;
    std::atomic<int> click_count = 0;
    std::thread count(counter, &click_count, &end_count);
    sleep(SLEEP_TIME);
    end_count = true;
    count.join();
    return click_count;
}

TEST(testPiEar, test_click) {
    std::atomic<int> cpm = 0;
    std::thread click(PiEar::mainloop_click, &cpm);

    std::array<int, 10> cpms = {6000, 5000, 3000, 2000, 1000, 500, 300, 150, 120, 60};
    for (auto item : cpms){
        cpm = item;
        EXPECT_NEAR(((double)cpm / (60.0 / (double)SLEEP_TIME)), run_test(), ((int)((double)cpm*MARGIN_OF_ERROR) + 0.5));
    }
    PiEar::kill_program = true;
    click.join();
    EXPECT_TRUE(PiEar::click);
}
