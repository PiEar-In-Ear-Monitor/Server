#ifndef PIEAR_SERVER_LOAD_DATA_HPP
#define PIEAR_SERVER_LOAD_DATA_HPP
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <utility>
#include <vector>
#include "channel.hpp"
namespace PiEar {
    /**
     * @brief Loads the data from a file
     * @param std::string file_name
     * @return std::string contents of the file
     */
    std::string get_file_contents(const std::string& view_path) {
        constexpr auto read_size = std::size_t(4096);
        auto stream = std::ifstream(view_path, std::ios::binary);
        auto out = std::string();
        auto buf = std::string(read_size, '\0');
        while (stream.readsome(&buf[0], read_size)) {
            out.append(buf, 0, stream.gcount());
        }
        out.append(buf, 0, stream.gcount());
        stream.close();
        return out;
    }
    /**
     * @brief Returns the channels from the string
     * @param json string containing the channels
     * @param key key under which the channels are stored
     * @return pointer to the vector of channels
     */
    std::vector<channel*> *process_array(std::string* json,  const std::string& key) {
        if (json->empty()) {
            return nullptr;
        }
        nlohmann::json data = nlohmann::json::parse(*json);
        auto *channel_vector = new std::vector<channel*>();
        for (const auto& channel : data[key]){
            auto *new_channel = new PiEar::channel(channel["piear_id"], "", channel["enabled"]);
            new_channel->channel_name = channel["channel_name"];
            channel_vector->push_back(new_channel);
        }
        return channel_vector;
    }
}
#endif //PIEAR_SERVER_LOAD_DATA_HPP
