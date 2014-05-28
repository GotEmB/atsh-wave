// dns_service_ref_wrap.h

#ifndef DNSSERVICEREFWRAP_H
#define DNSSERVICEREFWRAP_H

#include <node.h>
#include <node_object_wrap.h>
#include <dns_sd.h>

using namespace v8;

enum DNSServiceRefWrapType { DNSServiceAdvertisement, DNSServiceBrowser };

class DNSServiceRefWrap : public node::ObjectWrap {
public:
	static void Init();
	static void NewAdvertisement(const FunctionCallbackInfo<Value> &args);
	static void NewBrowser(const FunctionCallbackInfo<Value> &args);

private:
	DNSServiceRef *ref;
	DNSServiceRefWrapType type;

	explicit DNSServiceRefWrap(DNSServiceRef *ref, DNSServiceRefWrapType type);

	static void Terminate(const FunctionCallbackInfo<Value>& args);
	static Persistent<Function> AdvertisementConstructor;
	static Persistent<Function> BrowserConstructor;
	static void DNSServiceBrowseReply(DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *serviceName, const char *regtype, const char *replyDomain, void *context);
};

#endif