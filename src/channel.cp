//
// Created by alex on 2/7/22.
//

#include "channel.hpp"
#include <sstream>
#include <string>
#include <utility>

#define ADD_KEY_VAL_LIT(s, k, v) s << '"' << k << "\":" << v
#define ADD_KEY_VAL_STR(s, k, v) s << '"' << k << "\":\"" << v << '"'
#define COMMA(s) s << ','

namespace PiEar{
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