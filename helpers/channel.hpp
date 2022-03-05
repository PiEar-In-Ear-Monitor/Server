//
// Created by alex on 2/7/22.
//

#ifndef PIEAR_SERVER_CHANNEL_HPP
#define PIEAR_SERVER_CHANNEL_HPP

#define ADD_KEY_VAL_LIT(s, k, v) s << '"' << k << "\":" << v
#define ADD_KEY_VAL_STR(s, k, v) s << '"' << k << "\":\"" << v << '"'
#define COMMA(s) s << ','

#include <string>
#include <sstream>
#include <utility>

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
         * @param std::string channel name
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


    channel::channel(int pw_id, int pe_id, std::string c_name, bool en) :
            pipewire_id(pw_id), piear_id(pe_id), channel_name(std::move(c_name)), enabled(en) {}

    channel::operator std::basic_string<char>() const {
        std::ostringstream s;
        s << "{";
        ADD_KEY_VAL_LIT(s, "pipewire_id", pipewire_id); COMMA(s);
        ADD_KEY_VAL_LIT(s, "piear_id", piear_id); COMMA(s);
        ADD_KEY_VAL_STR(s, "channel_name", channel_name); COMMA(s);
        ADD_KEY_VAL_LIT(s, "enabled", (enabled?"true":"false"));
        s << "}";
        return s.str();
    }

    channel::operator bool() const{
        return enabled;
    }

    channel::operator int() const {
        const int final = piear_id;
        return final;
    }

    std::string channel::log_line() const {
        std::ostringstream s;
        s << "(PipeWire ID: " << pipewire_id << ") Channel " << piear_id;
        s << " named " << channel_name << (enabled?" is enabled":" is not enabled");
        return s.str();
    }

    bool operator==(channel c, int i) {
        return c.piear_id == i;
    }
}

#endif //PIEAR_SERVER_CHANNEL_HPP
