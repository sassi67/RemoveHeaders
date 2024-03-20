#pragma once

#include <httpserv.h>
#include <set>
#include <string>

class RemoveHeadersFactory : public IHttpModuleFactory
{
public:
	RemoveHeadersFactory(const std::set<std::string>& removeHeaders) : removeHeaders_(removeHeaders) {}
	HRESULT GetHttpModule(OUT CHttpModule** ppModule, IN IModuleAllocator* pAllocator) override;
	void Terminate() override;
private:
	std::set<std::string> removeHeaders_;
};

