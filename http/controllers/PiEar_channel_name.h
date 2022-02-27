#pragma once

#include <drogon/HttpSimpleController.h>

using namespace drogon;

namespace PiEar
{
class channel_name : public drogon::HttpSimpleController<channel_name>
{
  public:
    virtual void asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) override;
    PATH_LIST_BEGIN
    // list path definitions here;
    PATH_ADD("/path", Get);
    PATH_LIST_END
};
}
