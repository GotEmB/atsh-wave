// dns_service_ref_wrap.h

#ifndef DNSSERVICEREFWRAP_H
#define DNSSERVICEREFWRAP_H

#include <node.h>
#include <node_object_wrap.h>
#include <dns_sd.h>

class DNSServiceRefWrap : public node::ObjectWrap {
public:
	static void Init();
	static void NewInstance(const v8::FunctionCallbackInfo<v8::Value> &args);
private:
	DNSServiceRef *ref;
	explicit DNSServiceRefWrap(DNSServiceRef *ref);
	static void New(const v8::FunctionCallbackInfo<v8::Value> &args);
	static void Terminate(const v8::FunctionCallbackInfo<v8::Value>& args);
	static v8::Persistent<v8::Function> constructor;
};

#endif