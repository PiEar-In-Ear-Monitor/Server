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

/**
 * @file click.h
 * @brief Provides click functionality
 * @author Alex O'Connor
 * @date 12 December 2021
 */

#ifndef PIEAR_SERVER_CLICK_H
#define PIEAR_SERVER_CLICK_H

/**
 * Is a continual loop where output will indicate when the click should "click"
 * @param cpm Is an int pointer that indicates the clicks-per-minute - SHOULD NEVER EQUAL 0
 * @param output Is an bool pointer to the expected output
 * @param end_main Is a bool that when set true will end the otherwise infinite while loop
 */
void mainloop_click(int* cpm, bool* output, bool* end_main);

#endif //PIEAR_SERVER_CLICK_H
