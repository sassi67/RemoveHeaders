#include "pch.h"
#include "RemoveHeadersModule.h"

REQUEST_NOTIFICATION_STATUS RemoveHeadersModule::OnSendResponse(IN IHttpContext* pHttpContext, IN ISendResponseProvider* pProvider)
{
	// Retrieve a pointer to the response and test for an error
	if (auto* pHttpResponse = pHttpContext->GetResponse(); pHttpResponse != nullptr) {
		for (const auto& header : removeHeaders_) {
			if (const auto hr = pHttpResponse->DeleteHeader(header.c_str());  FAILED(hr)) {
				// Set the error status.
				pProvider->SetErrorStatus(hr);
				// End additional processing.
				return RQ_NOTIFICATION_FINISH_REQUEST;
			}
		}
	}

	// Return processing to the pipeline.
	return RQ_NOTIFICATION_CONTINUE;
}
