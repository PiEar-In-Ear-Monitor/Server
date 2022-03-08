#include "PiEar_bpm.h"

namespace PiEar {
    // Add definition of your processing function here
    void bpm::get_bpm(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback) const {
        //Verify the validity of the token, etc.
        //Read the database or cache to get user information
        Json::Value ret;
        ret["result"] = "ok";
        ret["bpm"] = 100;
        auto resp = drogon::HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }
    void bpm::post_bpm(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback, int new_bpm) const {
        //Verify the validity of the token, etc.
        //Read the database or cache to get user information
        LOG_DEBUG << "New BPM: " << new_bpm;
        Json::Value json_return;
        json_return["result"] = "ok";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(json_return);
        callback(resp);
    }
}
