#ifndef PIEAR_SERVER_LOAD_DATA_H
#define PIEAR_SERVER_LOAD_DATA_H

#include "channel.h"
#include <vector>

namespace PiEar {
    std::vector<PiEar::channel*> *parse_json(std::string);
    std::string *get_file_contents(std::string);
    std::vector<std::string*> *get_array_key(std::string*, const std::string&);
    std::vector<PiEar::channel*> *process_array(std::vector<std::string*>*);
}

#endif //PIEAR_SERVER_LOAD_DATA_H
