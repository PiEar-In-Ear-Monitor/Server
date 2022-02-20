#include "load-data.h"
#include "channel.h"
#include <utility>
#include <vector>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

namespace PiEar {
    std::string get_file_contents(std::string view_path) {
        constexpr auto read_size = std::size_t(4096);
        auto stream = std::ifstream(view_path.data());
        stream.exceptions(std::ios_base::badbit);

        auto out = std::string();
        auto buf = std::string(read_size, '\0');
        while (stream.read(& buf[0], read_size)) {
            out.append(buf, 0, stream.gcount());
        }
        out.append(buf, 0, stream.gcount());
        return out;
    }

    std::vector<channel*> *process_array(std::string* json,  const std::string& key) {
        nlohmann::json data = nlohmann::json::parse(*json);
        auto *channel_vector = new std::vector<channel*>();
        int i = 0;
        for (const auto& channel : data[key]){
            auto *new_channel = new PiEar::channel(channel["pipewire_id"], channel["piear_id"], channel["channel_name"], channel["enabled"]);
            channel_vector->push_back(new_channel);
        }
        return channel_vector;
    }
}