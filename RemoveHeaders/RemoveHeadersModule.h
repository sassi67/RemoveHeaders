#pragma once

#include <httpserv.h>
#include <set>
#include <string>

class RemoveHeadersModule : public CHttpModule
{
public:
	RemoveHeadersModule(const std::set<std::string>& removeHeaders) : removeHeaders_(removeHeaders) {}
	REQUEST_NOTIFICATION_STATUS OnSendResponse(IN IHttpContext* pHttpContext, IN ISendResponseProvider* pProvider) override;

private:
	std::set<std::string> removeHeaders_;
};
