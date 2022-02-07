//
// Created by alex on 2/6/22.
//

#include <iostream>
#include "json_data.h"
#include <boost/asio.hpp>
#include <json-c/json.h>

json_data::json_data(std::string file, int number_of_channels)  : filename(file){
    load_data(number_of_channels);
}

json_data::json_data(std::string file)  : filename(file){
    load_data(0);
}

json_data::json_data(int number_of_channels)  : filename(std::string(DEFAULT_FILE)){
    load_data(number_of_channels);
}

json_data::json_data()  : filename(std::string(DEFAULT_FILE)){
    load_data(0);
}

inline bool json_data::load_data(int number_of_channels) {
    data = json_object_from_file(filename.c_str());
    if (!data) {
        return load_default();
    }
    // TODO check number_of_channels
    return true;
}

inline bool json_data::load_default() {
    data = json_object_from_file(DEFAULT_FILE);
    if (!data) {
        return new_json();
    }
    return true;
}

inline bool json_data::new_json(){
    data = json_object_new_object();
    json_object *array = json_object_new_array();
    json_object_array_add(data, array);
    json_object_put(array);
    return (data && array);
}

std::string json_data::get_channel_name(int channel_index) {
//    struct array_list *arry = json_object_get_array(array);
//    for (int i = 0; i < arry->length; ++i) {
//        struct json_object *row = (struct json_object*)array_list_get_idx(arry,i);
//        if
//    }
}

bool json_data::set_channel_name(int channel_index, std::string *channel_name) {

}

bool json_data::toggle_channel(int channel_index) {

}

bool json_data::reset_channels() {

}