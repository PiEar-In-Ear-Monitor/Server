#include "load-data.h"
#include "channel.h"
#include <vector>
#include <fstream>
#include <iostream>

#define ADD_START_END_PAIR(b, e, bc, ec) b.push_back(bc); e.push_back(ec);

std::vector<PiEar::channel*> *parse_json(std::string file_path) {
    std::string *json_to_parse = get_file_contents(file_path);
    std::vector<std::string*> *json_array = get_array_key(json_to_parse, "channels");
    return process_array(json_array);
}

std::string *get_file_contents(std::string view_path) {
    constexpr auto read_size = std::size_t(4096);
    auto stream = std::ifstream(view_path.data());
    stream.exceptions(std::ios_base::badbit);

    auto out = std::string();
    auto buf = std::string(read_size, '\0');
    while (stream.read(& buf[0], read_size)) {
        out.append(buf, 0, stream.gcount());
    }
    out.append(buf, 0, stream.gcount());
    return new std::string(out);
}

std::vector<std::string*> *get_array_key(std::string *file_contents, std::string key) {
    std::size_t start = file_contents->find(key + ":[{") + key.length() + 3;
    std::size_t end = file_contents->find('}', start);
    std::vector<size_t[2]> object_bounds;
    while (start != std::string::npos and end != std::string::npos) {
        if (start != std::string::npos and end != std::string::npos){
            std::size_t tmp_array[] = {start, end};
            object_bounds.push_back(tmp_array);
        }
        start += file_contents->find('{', end);
        end = file_contents->find('}', start);
    }
    for (auto it = object_bounds.begin(); it != object_bounds.end(); it++)
        std::cout << *it;
}

std::vector<PiEar::channel*> *process_array(std::vector<std::string*>*) {
    ;
}
