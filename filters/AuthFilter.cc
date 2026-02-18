/**
 *
 *  AuthFilter.cc
 *
 */

#include "AuthFilter.h"
#include "utils/SessionUtils.h"
#include "utils/RequestHeaderUtils.h"
#include <drogon/HttpTypes.h>
#include <json/value.h>
#include <string>

using namespace drogon;
void AuthFilter::doFilter(const HttpRequestPtr &req,
                         FilterCallback &&fcb,
                         FilterChainCallback &&fccb)
{
    if (req->method() == HttpMethod::Options)
    {
        LOG_DEBUG<<"Options Method auto pass";
        fccb();
        return;
    }
    std::string auth = getAuthToken(req->getHeader("Authorization"));
    if (auth.length() == 0)
    {
        LOG_DEBUG<<"INVALID AUTH TOKEN";
        fccb();
        return;
    }
    LOG_DEBUG<<"auth "<<auth;
    //LOG_DEBUG<<"cookie"<<cookie;
    if (isSessionValid(auth))
    {
        LOG_DEBUG<<"Session Valid";
        fccb();
        return;
    }
    //Check failed
    auto res = drogon::HttpResponse::newHttpResponse();
    res->setStatusCode(drogon::k401Unauthorized);
    fcb(res);
}
