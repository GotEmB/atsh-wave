// dns_service_ref_wrap.cc

#include <node.h>
#include <cstdlib>
#include <cstring>
#include "dns_service_ref_wrap.h"

using namespace v8;

Persistent<Function> DNSServiceRefWrap::constructor;

DNSServiceRefWrap::DNSServiceRefWrap(DNSServiceRef *ref) : ref(ref) {
}

void DNSServiceRefWrap::Init() {
	Isolate* isolate = Isolate::GetCurrent();
	// Prepare constructor template
	Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
	tpl->SetClassName(String::NewFromUtf8(isolate, "DNSService"));
	tpl->InstanceTemplate()->SetInternalFieldCount(1);

	// Prototype
	NODE_SET_PROTOTYPE_METHOD(tpl, "terminate", Terminate);

	constructor.Reset(isolate, tpl->GetFunction());
}

void DNSServiceRefWrap::New(const FunctionCallbackInfo<Value> &args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	if (args.IsConstructCall()) {
		// Invoked as constructor: `new DNSServiceRefWrap(...)`
		DNSServiceRef *ref = (DNSServiceRef *)malloc(sizeof(DNSServiceRef));
		memset(ref, 0, sizeof(DNSServiceRef));
		String::Utf8Value regType(args[0]->ToString());
		DNSServiceRegister(ref, 0, 0, NULL, *regType, NULL, NULL, args[1]->Int32Value(), 0, NULL, NULL, NULL);
		DNSServiceRefWrap* obj = new DNSServiceRefWrap(ref);
		args.This()->Set(String::NewSymbol("service"), args[0]->ToString(), ReadOnly);
		args.This()->Set(String::NewSymbol("port"), Number::New(args[1]->Int32Value()), ReadOnly);
		args.This()->Set(String::NewSymbol("advertising"), True(isolate), ReadOnly);
		obj->Wrap(args.This());
		args.GetReturnValue().Set(args.This());
	} else {
		// Invoked as plain function `DNSServiceRefWrap(...)`, turn into construct call.
		const int argc = 2;
		Local<Value> argv[argc] = { args[0], args[1] };
		Local<Function> cons = Local<Function>::New(isolate, constructor);
		args.GetReturnValue().Set(cons->NewInstance(argc, argv));
	}
}

void DNSServiceRefWrap::NewInstance(const FunctionCallbackInfo<Value> &args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	const int argc = 2;
	Local<Value> argv[argc] = { args[0], args[1] };
	Local<Function> cons = Local<Function>::New(isolate, constructor);
	args.GetReturnValue().Set(cons->NewInstance(argc, argv));
}

void DNSServiceRefWrap::Terminate(const v8::FunctionCallbackInfo<v8::Value> &args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	DNSServiceRefWrap* obj = ObjectWrap::Unwrap<DNSServiceRefWrap>(args.Holder());
	if (obj->ref) {
		DNSServiceRefDeallocate(*obj->ref);
		free(obj->ref);
		obj->ref = NULL;
		args.Holder()->ForceSet(String::NewSymbol("advertising"), False(isolate), ReadOnly);
		args.GetReturnValue().Set(args.Holder());
	} else {
		ThrowException(Exception::ReferenceError(v8::String::NewFromUtf8(isolate, "Service already terminated.")));
	}
}