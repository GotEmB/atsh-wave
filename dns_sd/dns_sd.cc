#include <node.h>
#include <v8.h>
#include <dns_sd.h>
#include "dns_service_ref_wrap.h"

using namespace v8;
using namespace node;

void NewAdvertisement(const FunctionCallbackInfo<Value> &args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	DNSServiceRefWrap::NewAdvertisement(args);
}

void NewBrowser(const FunctionCallbackInfo<Value> &args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	
	DNSServiceRefWrap::NewBrowser(args);
}

void init(Handle<Object> exports) {
	DNSServiceRefWrap::Init();
	NODE_SET_METHOD(exports, "DNSServiceAdvertisement", NewAdvertisement);
	NODE_SET_METHOD(exports, "DNSServiceBrowser", NewBrowser);
}

NODE_MODULE(dns_sd, init)