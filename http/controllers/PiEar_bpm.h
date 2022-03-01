#pragma once
#include <drogon/HttpController.h>

namespace PiEar {
class bpm : public drogon::HttpController<bpm> {
    public:
        METHOD_LIST_BEGIN
            METHOD_ADD(bpm::get_bpm, "/bpm", drogon::Get);
            ADD_METHOD_VIA_REGEX(bpm::post_bpm, "/bpm/[0-9]*", drogon::Post);
        METHOD_LIST_END
        void get_bpm(const drogon::HttpRequestPtr& req, std::function<void (const drogon::HttpResponsePtr &)> &&callback) const;
        void post_bpm(const drogon::HttpRequestPtr& req, std::function<void (const drogon::HttpResponsePtr &)> &&callback, int new_bpm) const;
    };
}
