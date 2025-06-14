#include "pingmethod_http.h"
#include <rapidjson/document.h>
#include "utils/wsnet_logger.h"
#include <skyr/url.hpp>
#include "utils/utils.h"

namespace wsnet {

PingMethodHttp::PingMethodHttp(WSNetHttpNetworkManager *httpNetworkManager, std::uint64_t id, const std::string &ip, const std::string &hostname, bool isParallelPing,
                               PingFinishedCallback callback, PingMethodFinishedCallback pingMethodFinishedCallback, WSNetAdvancedParameters *advancedParameters) :
    IPingMethod(id, ip, hostname, isParallelPing, callback, pingMethodFinishedCallback),
    httpNetworkManager_(httpNetworkManager),
    advancedParameters_(advancedParameters)
{
}

PingMethodHttp::~PingMethodHttp()
{
    if (request_)
        request_->cancel();
}

void PingMethodHttp::ping(bool isFromDisconnectedVpnState)
{
    if (!utils::isIpAddress(ip_)) {
        g_logger->error("PingMethodHttp::ping incorrect IP-address: {}", ip_);
        callFinished();
        return;
    }

    try {
        auto url = skyr::url(hostname_);
    }
    catch(...) {
        g_logger->error("PingMethodHttp::ping incorrect URL: {}", hostname_);
        callFinished();
        return;
    }


    isFromDisconnectedVpnState_ = isFromDisconnectedVpnState;
    auto httpRequest = httpNetworkManager_->createGetRequest(hostname_, PING_TIMEOUT, false);
    httpRequest->setOverrideIp(ip_);
    // We add all ips to the firewall exceptions at once in the client before we ping, so there is no need to do it in HttpNetworkManager
    httpRequest->setIsWhiteListIps(false);
    httpRequest->setExtraTLSPadding(advancedParameters_->isAPIExtraTLSPadding());
    using namespace std::placeholders;
    request_ = httpNetworkManager_->executeRequest(httpRequest, 0, std::bind(&PingMethodHttp::onNetworkRequestFinished, this, _1, _2, _3, _4));
}

void PingMethodHttp::onNetworkRequestFinished(std::uint64_t requestId, std::uint32_t elapsedMs,
                                              std::shared_ptr<WSNetRequestError> error, const std::string &data)
{
    request_.reset();
    if (error->isSuccess()) {
        auto timems = parseReplyString(data);
        if (timems != -1) {
            isSuccess_ = true;
            timeMs_ = round(timems / 1000.0);
        }
    }
    callFinished();
}

int PingMethodHttp::parseReplyString(const std::string &data)
{
    using namespace rapidjson;
    Document doc;
    doc.Parse(data.c_str());
    if (doc.HasParseError() || !doc.IsObject()) {
        g_logger->error("PingMethodHttp::parseReplyString incorrect JSON: {}", data);
        return -1;
    }
    auto jsonObject = doc.GetObject();
    if (jsonObject.HasMember("rtt")) {
        std::string value = jsonObject["rtt"].GetString();
        if (!value.empty()) {
            try {
                int res = std::stoi(value);
                if (res == 0) {
                    // If the result is exactly 0, either server gave us an invalid value, or we failed to parse it.  Treat it as invalid
                    g_logger->warn("PingMethodHttp::parseReplyString, rtt value is 0: {}", value);
                    return -1;
                } else {
                    if (res < 1000) {  // is result < 1 ms, log it as a warning
                        g_logger->warn("PingMethodHttp::parseReplyString, rtt value < 1000: {}", value);
                    }
                    return res;
                }
            } catch(...) {
                // goto end of function and log error
            }
        }
    }
    g_logger->error("PingMethodHttp::parseReplyString incorrect JSON: {}", data);
    return -1;
}

} // namespace wsnet
