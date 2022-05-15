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
#include <boost/beast/core/detail/base64.hpp>

namespace PiEar {
    class channel {
    private:
    public:
        static std::string base64_encode(boost::beast::string_view s) {
            std::uint8_t const* data = reinterpret_cast < std::uint8_t const*> (s.data());
            std::size_t len = s.size();
            std::string dest;
            dest.resize(boost::beast::detail::base64::encoded_size(len));
            dest.resize(boost::beast::detail::base64::encode(&dest[0], data, len));
            return dest;
        }
        int pipewire_id;            //!< ID of channel in pipewire
        int piear_id;               //!< ID of channel in piear
        std::string channel_name;   //!< Channel name
        bool enabled;               //!< If the channel is enabled in piear
        /**
         * @param int pipewire_id
         * @param int piear_id
         * @param std::string channel name
         * @param bool enabled
         */
        channel(int pw_id, int pe_id, const std::string& c_name, bool en) :
                pipewire_id(pw_id), piear_id(pe_id), channel_name(base64_encode(c_name)), enabled(en) {}
        /**
         * The default `std::string` conversion is to
         * a JSON object
         * @return std::string JSON representation
         */
        explicit operator std::basic_string<char>() const {
            std::ostringstream s;
            s << "{";
            ADD_KEY_VAL_LIT(s, "pipewire_id", pipewire_id); COMMA(s);
            ADD_KEY_VAL_LIT(s, "piear_id", piear_id); COMMA(s);
            ADD_KEY_VAL_STR(s, "channel_name", channel_name); COMMA(s);
            ADD_KEY_VAL_LIT(s, "enabled", (enabled?"true":"false"));
            s << "}";
            return s.str();
        }
        /**
         * The default `bool` conversion is to
         * return if the channel is enabled
         * @return bool `enabled`
         */
        //explicit operator bool() const;
        explicit operator bool() const{
            return enabled;
        }
        /**
         * The default `int` representation is to
         * return the piear_id of the channel
         * @return int `piear_id`
         */
        explicit operator int() const {
            const int final = piear_id;
            return final;
        }
        /**
         * Created during testing, compares
         * channel object to in, by comparing
         * piear_id to the other int
         * @return bool `piear_id` == `int`
         */
        bool operator==(int rhs) const {
            return piear_id == rhs;
        }
    };
};
#endif //PIEAR_SERVER_CHANNEL_HPP
