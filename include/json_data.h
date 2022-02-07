//
// Created by alex on 2/6/22.
//

#ifndef PIEAR_SERVER_JSON_DATA_H
#define PIEAR_SERVER_JSON_DATA_H

#include <string>
#include <json-c/json.h>

#define DEFAULT_FILE "piear_default.json"

/**
 * JSON structure:
 * {
 *     "channels": [
 *         {
 *             "id": <int>,
 *             "enable": <bool>,
 *             "name": "Channel Name"
 *         },
 *         ...
 *     ]
 * }
 */
class json_data {
private:
    inline bool load_data(int);
    inline bool load_default();
    inline bool new_json();
    void init();
public:
    std::string filename;
    json_object *data;
    std::string get_channel_name(int);
    bool set_channel_name(int, std::string*);
    explicit json_data(std::string, int);
    explicit json_data(std::string);
    bool toggle_channel(int);
    explicit json_data(int);
    bool reset_channels();
    explicit json_data();
};


#endif //PIEAR_SERVER_JSON_DATA_H
