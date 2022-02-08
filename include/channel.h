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
        int pipewire_id;
        int piear_id;
        std::string channel_name;
        bool enabled;
        [[nodiscard]] std::string log_line() const;
        channel(int, int, std::string, bool);
        explicit operator std::basic_string<char>() const;
        explicit operator bool() const;
        explicit operator int() const ;
        friend bool operator ==(channel, int);
    };
}

#endif //PIEAR_SERVER_CHANNEL_H
