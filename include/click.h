/**
 * @file click.h
 * @brief Provides click functionality
 * @author Alex O'Connor
 * @date 12 December 2021
 */

#ifndef PIEAR_SERVER_CLICK_H
#define PIEAR_SERVER_CLICK_H

namespace PiEar {
    /**
     * Is a continual loop where output will indicate when the click should "click"
     * @param cpm Is an int pointer that indicates the clicks-per-minute - SHOULD NEVER EQUAL 0
     * @param output Is an bool pointer to the expected output
     * @param end_main Is a bool that when set true will end the otherwise infinite while loop
     */
    void mainloop_click(int *cpm, bool *output, bool *end_main);
}

#endif //PIEAR_SERVER_CLICK_H
