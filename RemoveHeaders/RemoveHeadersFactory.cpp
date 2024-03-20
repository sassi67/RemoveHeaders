#include "pch.h"
#include "RemoveHeadersFactory.h"
#include "RemoveHeadersModule.h"

HRESULT RemoveHeadersFactory::GetHttpModule(OUT CHttpModule** ppModule, IN IModuleAllocator* pAllocator)
{
	UNREFERENCED_PARAMETER(pAllocator);

	RemoveHeadersModule* pModule = new RemoveHeadersModule(removeHeaders_);

	if (pModule == nullptr) {
		// Return an error if the factory cannot create the instance.
		return HRESULT_FROM_WIN32(ERROR_NOT_ENOUGH_MEMORY);
	}

	// Return a pointer to the module.
	*ppModule = pModule;
	pModule = nullptr;
	return S_OK;
}

void RemoveHeadersFactory::Terminate()
{
    delete this;
}
