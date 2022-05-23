#include <cstring>
#include <gtest/gtest.h>
#include <string>
#include "ring-buffer.hpp"

TEST(testPiEar, ring_buffer_basic) {
    int chunk_size = 2;
    int *input = (int*) malloc(sizeof(int) * chunk_size);
    int *output;
    PiEar::ringBuffer<int> rb(10, chunk_size);

    EXPECT_EQ(rb.size(), 10);
    EXPECT_EQ(rb.capacity(), 5);

    input[0] = 1;
    input[1] = 2;
    rb.push(input);
    output = rb.pop();
    EXPECT_EQ(output[0], 1);
    EXPECT_EQ(output[1], 2);
    free(output);
    free(input);
}
TEST(testPiEar, ring_buffer_too_full) {
    int chunk_size = 5;
    int *input = (int*) malloc(sizeof(int) * chunk_size);
    int *output;
    PiEar::ringBuffer<int> rb(10, chunk_size);
    memset(input, 0, sizeof(int) * chunk_size);
    rb.push(input);
    memset(input, 1, sizeof(int) * chunk_size);
    rb.push(input);
    rb.push(input);
    output = rb.pop();
    EXPECT_EQ(memcmp(output, input, 5 * sizeof(int)), 0);
    free(output);
    output = rb.pop();
    EXPECT_EQ(memcmp(output, input, 5 * sizeof(int)), 0);
    free(output);
    free(input);
}
TEST(testPiEar, ring_buffer_empty) {
    int chunk_size = 5;
    int *output;
    PiEar::ringBuffer<int> rb(10, chunk_size);
    output = rb.pop();
    EXPECT_EQ(output, nullptr);
    free(output);
}
