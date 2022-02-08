//******************************************************************************
// Copyright © 2021  Alexander O'Connor
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