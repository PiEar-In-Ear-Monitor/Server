#include "gtest/gtest.h"
#include "ring-buffer.hpp"
#include <string>

TEST(testPiEar, ring_buffer_basic) {
    // Setup for test
    int chunk_size = 2;
    int *input = (int*) malloc(sizeof(int) * chunk_size);
    int *output;
    PiEar::ringBuffer<int> rb(10, chunk_size);

    // Basic tests
    EXPECT_EQ(rb.size(), 10);
    EXPECT_EQ(rb.capacity(), 5);

    // Push and Pop
    input[0] = 1;
    input[1] = 2;
    rb.push(input);
    output = rb.pop();
    EXPECT_EQ(output[0], 1);
    EXPECT_EQ(output[1], 2);
    free(output);
}

TEST(testPiEar, ring_buffer_too_full) {
    // Setup for test
    int chunk_size = 5;
    int *input = (int*) malloc(sizeof(int) * chunk_size);
    int *output;
    PiEar::ringBuffer<int> rb(10, chunk_size);

    // Assign for first test
    input[0] = 0;
    input[1] = 1;
    input[2] = 2;
    input[3] = 3;
    input[4] = 4;
    rb.push(input);
    input[0] = 5;
    input[1] = 6;
    input[2] = 7;
    input[3] = 8;
    input[4] = 9;
    rb.push(input);
    rb.push(input);
    output = rb.pop();
    EXPECT_EQ(output[0], 5);
    EXPECT_EQ(output[1], 6);
    EXPECT_EQ(output[2], 7);
    EXPECT_EQ(output[3], 8);
    EXPECT_EQ(output[4], 9);
    free(output);
    output = rb.pop();
    EXPECT_EQ(output[0], 5);
    EXPECT_EQ(output[1], 6);
    EXPECT_EQ(output[2], 7);
    EXPECT_EQ(output[3], 8);
    EXPECT_EQ(output[4], 9);
    free(output);
}

TEST(testPiEar, ring_buffer_empty) {
    // Setup for test
    int chunk_size = 5;
    int *output;
    PiEar::ringBuffer<int> rb(10, chunk_size);
    output = rb.pop();
    EXPECT_EQ(output, nullptr);
}
