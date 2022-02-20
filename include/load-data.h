#ifndef PIEAR_SERVER_LOAD_DATA_H
#define PIEAR_SERVER_LOAD_DATA_H

#include "channel.h"
#include <vector>

namespace PiEar {
    std::string get_file_contents(std::string);
    std::vector<PiEar::channel *> *process_array(std::string *, const std::string &);
}

#endif //PIEAR_SERVER_LOAD_DATA_H
