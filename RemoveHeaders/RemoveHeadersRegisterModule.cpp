#include "pch.h"

#include <ahadmin.h>
#include <comutil.h>
#include <oleauto.h>
#include <winerror.h>
#include <httpserv.h>
#include <set>
#include <string>
#include <oaidl.h>

#include "RemoveHeadersFactory.h"

std::set<std::string> removeHeaders;

HRESULT __stdcall RegisterModule(DWORD dwServerVersion, IHttpModuleRegistrationInfo* pModuleInfo, IHttpServer* pGlobalInfo)
{
	UNREFERENCED_PARAMETER(dwServerVersion);
	HRESULT hr = S_OK;

	// Get the configuration object
	IAppHostAdminManager* pMgr = pGlobalInfo->GetAdminManager();
	if (pMgr == nullptr) {
		return hr;
	}

	// Get the <removeHeaders> section of the configuration
	IAppHostElement* pRemoveHeadersElement = nullptr;
	BSTR bstrSectionName = SysAllocString(L"system.webServer/removeHeaders");
	BSTR bstrPath = SysAllocString(L"MACHINE/WEBROOT/APPHOST");
	hr = pMgr->GetAdminSection(bstrSectionName, bstrPath, &pRemoveHeadersElement);
	SysFreeString(bstrSectionName);
	SysFreeString(bstrPath);
	if (FAILED(hr) || (&pRemoveHeadersElement == nullptr)) {
		return hr;
	}

	// Get the remove headers element collection
	IAppHostElementCollection* pHeaderElements = nullptr;
	pRemoveHeadersElement->get_Collection(&pHeaderElements);
	if (FAILED(hr)) {
		return hr;
	}

	// Get the count of elements
	DWORD dwHeaderElementsCount = 0;
	hr = pHeaderElements->get_Count(&dwHeaderElementsCount);
	if (FAILED(hr) || (&dwHeaderElementsCount == nullptr)) {
		return hr;
	}

	IAppHostElement* pHeaderElement = nullptr;
	IAppHostPropertyCollection* pHeaderElementProperties = nullptr;
	IAppHostProperty* pHeaderElementNameProperty = nullptr;

	// Loop over header elements
	VARIANT vtHeaderNameValue;
	VARIANT vtHeaderNameProperty;
	vtHeaderNameProperty.vt = VT_BSTR;
	vtHeaderNameProperty.bstrVal = SysAllocString(L"name");
	VARIANT vtLoopIndex;
	vtLoopIndex.vt = VT_I2;

	for (DWORD i = 0; i < dwHeaderElementsCount; i++) {
		vtLoopIndex.iVal = i;

		// Get the current header element
		hr = pHeaderElements->get_Item(vtLoopIndex, &pHeaderElement);
		if (FAILED(hr) || (&pHeaderElement == nullptr)) {
			SysFreeString(vtHeaderNameProperty.bstrVal);
			return hr;
		}

		// Get the child elements
		hr = pHeaderElement->get_Properties(&pHeaderElementProperties);
		if (FAILED(hr) || (&pHeaderElementProperties == nullptr)) {
			SysFreeString(vtHeaderNameProperty.bstrVal);
			return hr;
		}

		hr = pHeaderElementProperties->get_Item(vtHeaderNameProperty, &pHeaderElementNameProperty);
		if (FAILED(hr) || (pHeaderElementNameProperty == nullptr)) {
			SysFreeString(vtHeaderNameProperty.bstrVal);
			return hr;
		}

		hr = pHeaderElementNameProperty->get_Value(&vtHeaderNameValue);
		if (FAILED(hr)) {
			SysFreeString(vtHeaderNameProperty.bstrVal);
			return hr;
		}

		// Add header name to headers set
		_bstr_t bstrHeader(vtHeaderNameValue.bstrVal);
		removeHeaders.insert((std::string)bstrHeader);

		// loop cleanup
		if (pHeaderElementNameProperty != nullptr) {
			pHeaderElementNameProperty->Release();
			pHeaderElementNameProperty = nullptr;
		}
		if (pHeaderElementProperties != nullptr) {
			pHeaderElementProperties->Release();
			pHeaderElementProperties = nullptr;
		}
		if (pHeaderElement != nullptr) {
			pHeaderElement->Release();
			pHeaderElement = nullptr;
		}
	}

	// final cleanup
	SysFreeString(vtHeaderNameProperty.bstrVal);
	if (pHeaderElements != nullptr) {
		pHeaderElements->Release();
		pHeaderElements = nullptr;
	}
	if (pRemoveHeadersElement != nullptr) {
		pRemoveHeadersElement->Release();
		pRemoveHeadersElement = nullptr;
	}
	if (pMgr != nullptr) {
		pMgr = nullptr;
	}

	// Set the request notifications
	hr = pModuleInfo->SetRequestNotifications(new RemoveHeadersFactory(removeHeaders), RQ_SEND_RESPONSE, 0);
	if (FAILED(hr)) {
		return hr;
	}

	// Set the request priority.
	// Note: The priority levels are inverted for RQ_SEND_RESPONSE notifications.
	hr = pModuleInfo->SetPriorityForRequestNotification(RQ_SEND_RESPONSE, PRIORITY_ALIAS_FIRST);
	if (FAILED(hr)) {
		return hr;
	}

	return S_OK;
}