#include "gtest/gtest.h"
#include "click.h"
#include <thread>
#include <array>

#define SLEEP_TIME      2
#define MARGIN_OF_ERROR 0.01

void counter(int *count, bool *click, bool *end) {
    while (!*end) {
        if (*click) {
            (*count)++;
            while (*click) {}
        }
    }
}

int run_test(int cpm, bool *active) {
    bool end_count = false;
    int click_count = 0;
    std::thread count(counter, &click_count, active, &end_count);
    sleep(SLEEP_TIME);
    end_count = true;
    count.join();
    return click_count;
}

TEST(testPiEar, testClick) {
    int cpm = 0;
    bool active = false, end_click = false;
    std::thread click(mainloop_click, &cpm, &active, &end_click);

    std::array<int, 10> cpms = {6000, 5000, 3000, 2000, 1000, 500, 300, 150, 120, 60};
    for (auto item : cpms){
        cpm = item;
        EXPECT_NEAR(((double)cpm / (60.0 / (double)SLEEP_TIME)), run_test(cpm, &active), ((double)cpm*MARGIN_OF_ERROR));
    }
    end_click = true;
    click.join();
    EXPECT_TRUE(active);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}