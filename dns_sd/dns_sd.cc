#include <node.h>
#include <v8.h>
#include <dns_sd.h>
#include "dns_service_ref_wrap.h"

using namespace v8;
using namespace node;

void Method(const FunctionCallbackInfo<Value> &args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	args.GetReturnValue().Set(String::NewFromUtf8(isolate, "world"));
}

void RegisterService(const FunctionCallbackInfo<Value> &args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	DNSServiceRefWrap::NewInstance(args);

	//refWrap->returnWrap(args);
	//args.GetReturnValue().Set(NanObjectWrapHandle(refWrap));
}

void init(Handle<Object> exports) {
	DNSServiceRefWrap::Init();
	NODE_SET_METHOD(exports, "hello", Method);
	NODE_SET_METHOD(exports, "registerService", RegisterService);
}

NODE_MODULE(dns_sd, init)