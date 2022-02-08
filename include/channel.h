//
// Created by alex on 2/7/22.
//

#ifndef PIEAR_SERVER_CHANNEL_H
#define PIEAR_SERVER_CHANNEL_H

#include <string>

namespace PiEar {
    class channel {
    private:
    public:
        int pipewire_id;            //!< ID of channel in pipewire
        int piear_id;               //!< ID of channel in piear
        std::string channel_name;   //!< Channel name
        bool enabled;               //!< If the channel is enabled in piear
        /**
         * Returns a line representation of the
         * object, useful for logging
         * @return std::string single line
         */
        [[nodiscard]] std::string log_line() const;
        /**
         * @param int pipewire_id
         * @param int piear_id
         * @param std::string file_path
         * @param bool enabled
         */
        channel(int, int, std::string, bool);
        /**
         * The default `std::string` conversion is to
         * a JSON object
         * @return std::string JSON representation
         */
        explicit operator std::basic_string<char>() const;
        /**
         * The default `bool` conversion is to
         * return if the channel is enabled
         * @return bool `enabled`
         */
        explicit operator bool() const;
        /**
         * The default `int` representation is to
         * return the piear_id of the channel
         * @return int `piear_id`
         */
        explicit operator int() const ;
        /**
         * Created during testing, compares
         * channel object to in, by comparing
         * piear_id to the other int
         * @return bool `piear_id` == `int`
         */
        friend bool operator ==(channel, int);
    };
}

#endif //PIEAR_SERVER_CHANNEL_H
