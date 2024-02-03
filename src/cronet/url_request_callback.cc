#include "url_request_callback.h"

#include <iostream>

UrlRequestCallback::UrlRequestCallback()
    : callback_(Cronet_UrlRequestCallback_CreateWith(
          UrlRequestCallback::OnRedirectReceived,
          UrlRequestCallback::OnResponseStarted,
          UrlRequestCallback::OnReadCompleted,
          UrlRequestCallback::OnSucceeded,
          UrlRequestCallback::OnFailed,
          UrlRequestCallback::OnCanceled)) {
  Cronet_UrlRequestCallback_SetClientContext(callback_, this);
}

UrlRequestCallback::~UrlRequestCallback() {
  Cronet_UrlRequestCallback_Destroy(callback_);
}

Cronet_UrlRequestCallbackPtr UrlRequestCallback::GetUrlRequestCallback() {
  return callback_;
}

void UrlRequestCallback::OnRedirectReceived(
    Cronet_UrlRequestPtr request,
    Cronet_UrlResponseInfoPtr info,
    Cronet_String newLocationUrl) {
  std::cout << "OnRedirectReceived called: " << newLocationUrl << std::endl;
  Cronet_UrlRequest_FollowRedirect(request);
}

void UrlRequestCallback::OnResponseStarted(
    Cronet_UrlRequestPtr request,
    Cronet_UrlResponseInfoPtr info) {
  std::cout << "OnResponseStarted called." << std::endl;
  std::cout << "HTTP Status: "
            << Cronet_UrlResponseInfo_http_status_code_get(info) << " "
            << Cronet_UrlResponseInfo_http_status_text_get(info) << std::endl;
  // Create and allocate 32kb buffer.
  Cronet_BufferPtr buffer = Cronet_Buffer_Create();
  Cronet_Buffer_InitWithAlloc(buffer, 32 * 1024);
  // Started reading the response.
  Cronet_UrlRequest_Read(request, buffer);
}

void UrlRequestCallback::OnReadCompleted(Cronet_UrlRequestPtr request,
                                               Cronet_UrlResponseInfoPtr info,
                                               Cronet_BufferPtr buffer,
                                               uint64_t bytes_read) {
  std::cout << "OnReadCompleted called: " << bytes_read << " bytes read."
            << std::endl;
  std::string last_read_data(
      reinterpret_cast<char*>(Cronet_Buffer_GetData(buffer)), bytes_read);
  response_as_string_ += last_read_data;
  // Continue reading the response.
  Cronet_UrlRequest_Read(request, buffer);
}

void UrlRequestCallback::OnSucceeded(Cronet_UrlRequestPtr request,
                                     Cronet_UrlResponseInfoPtr info) {
  std::cout << "OnSucceeded called." << std::endl;
  status_code = Cronet_UrlResponseInfo_http_status_code_get(info); 
  SignalDone(true);
}

void UrlRequestCallback::OnFailed(Cronet_UrlRequestPtr request,
                                        Cronet_UrlResponseInfoPtr info,
                                        Cronet_ErrorPtr error) {
  std::cout << "OnFailed called: " << Cronet_Error_message_get(error)
            << std::endl;
  last_error_message_ = Cronet_Error_message_get(error);
  SignalDone(false);
}

void UrlRequestCallback::OnCanceled(Cronet_UrlRequestPtr request,
                                          Cronet_UrlResponseInfoPtr info) {
  std::cout << "OnCanceled called." << std::endl;
  SignalDone(false);
}

/* static */
UrlRequestCallback* UrlRequestCallback::GetThis(
    Cronet_UrlRequestCallbackPtr self) {
  return static_cast<UrlRequestCallback*>(
      Cronet_UrlRequestCallback_GetClientContext(self));
}

/* static */
void UrlRequestCallback::OnRedirectReceived(
    Cronet_UrlRequestCallbackPtr self,
    Cronet_UrlRequestPtr request,
    Cronet_UrlResponseInfoPtr info,
    Cronet_String newLocationUrl) {
  GetThis(self)->OnRedirectReceived(request, info, newLocationUrl);
}

/* static */
void UrlRequestCallback::OnResponseStarted(
    Cronet_UrlRequestCallbackPtr self,
    Cronet_UrlRequestPtr request,
    Cronet_UrlResponseInfoPtr info) {
  GetThis(self)->OnResponseStarted(request, info);
}

/* static */
void UrlRequestCallback::OnReadCompleted(
    Cronet_UrlRequestCallbackPtr self,
    Cronet_UrlRequestPtr request,
    Cronet_UrlResponseInfoPtr info,
    Cronet_BufferPtr buffer,
    uint64_t bytesRead) {
  GetThis(self)->OnReadCompleted(request, info, buffer, bytesRead);
}

/* static */
void UrlRequestCallback::OnSucceeded(Cronet_UrlRequestCallbackPtr self,
                                           Cronet_UrlRequestPtr request,
                                           Cronet_UrlResponseInfoPtr info) {
  GetThis(self)->OnSucceeded(request, info);
}

/* static */
void UrlRequestCallback::OnFailed(Cronet_UrlRequestCallbackPtr self,
                                        Cronet_UrlRequestPtr request,
                                        Cronet_UrlResponseInfoPtr info,
                                        Cronet_ErrorPtr error) {
  GetThis(self)->OnFailed(request, info, error);
}

/* static */
void UrlRequestCallback::OnCanceled(Cronet_UrlRequestCallbackPtr self,
                                          Cronet_UrlRequestPtr request,
                                          Cronet_UrlResponseInfoPtr info) {
  GetThis(self)->OnCanceled(request, info);
}
