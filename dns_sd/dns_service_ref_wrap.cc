// dns_service_ref_wrap.cc

#include <node.h>
#include <cstdlib>
#include <cstring>
#include <map>
#include <thread>
#include "dns_service_ref_wrap.h"

using namespace v8;
using namespace node;

std::map<DNSServiceRef *, Local<Object> > refMap;

Persistent<Function> DNSServiceRefWrap::AdvertisementConstructor;
Persistent<Function> DNSServiceRefWrap::BrowserConstructor;

DNSServiceRefWrap::DNSServiceRefWrap(DNSServiceRef *ref, DNSServiceRefWrapType type) : ref(ref), type(type) { }

void DNSServiceRefWrap::Init() {
	Isolate* isolate = Isolate::GetCurrent();

	Local<FunctionTemplate> advertisementTemplate = FunctionTemplate::New(NewAdvertisement);
	Local<FunctionTemplate> browserTemplate = FunctionTemplate::New(NewBrowser);
	
	advertisementTemplate->SetClassName(String::NewFromUtf8(isolate, "DNSServiceAdvertisement"));
	advertisementTemplate->InstanceTemplate()->SetInternalFieldCount(1);
	browserTemplate->SetClassName(String::NewFromUtf8(isolate, "DNSServiceBrowser"));
	browserTemplate->InstanceTemplate()->SetInternalFieldCount(1);

	NODE_SET_PROTOTYPE_METHOD(advertisementTemplate, "terminate", Terminate);
	NODE_SET_PROTOTYPE_METHOD(browserTemplate, "terminate", Terminate);

	AdvertisementConstructor.Reset(isolate, advertisementTemplate->GetFunction());
	BrowserConstructor.Reset(isolate, browserTemplate->GetFunction());
}

void DNSServiceRefWrap::NewAdvertisement(const FunctionCallbackInfo<Value> &args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	if (args.IsConstructCall()) {
		DNSServiceRef *ref = (DNSServiceRef *)malloc(sizeof(DNSServiceRef));
		memset(ref, 0, sizeof(DNSServiceRef));
		String::Utf8Value regType(args[0]->ToString());
		DNSServiceRegister(ref, 0, 0, NULL, *regType, NULL, NULL, args[1]->Int32Value(), 0, NULL, NULL, NULL);

		DNSServiceRefWrap* obj = new DNSServiceRefWrap(ref, DNSServiceAdvertisement);
		obj->Wrap(args.This());
		refMap[ref] = Local<Object>::New(isolate, args.This());

		args.This()->Set(String::NewSymbol("service"), args[0]->ToString(), ReadOnly);
		args.This()->Set(String::NewSymbol("port"), Number::New(args[1]->Int32Value()), ReadOnly);
		args.This()->Set(String::NewSymbol("advertising"), True(isolate), ReadOnly);
		args.GetReturnValue().Set(args.This());
	} else {
		const int argc = 2;
		Local<Value> argv[argc] = { args[0], args[1] };
		Local<Function> cons = Local<Function>::New(isolate, AdvertisementConstructor);
		args.GetReturnValue().Set(cons->NewInstance(argc, argv));
	}
}

void DNSServiceRefWrap::NewBrowser(const FunctionCallbackInfo<Value> &args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	if (args.IsConstructCall()) {
		DNSServiceRef *ref = (DNSServiceRef *)malloc(sizeof(DNSServiceRef));
		memset(ref, 0, sizeof(DNSServiceRef));
		String::Utf8Value regType(args[0]->ToString());
		DNSServiceBrowse(ref, 0, 0, *regType, NULL, &DNSServiceRefWrap::DNSServiceBrowseReply, NULL);

		DNSServiceRefWrap* obj = new DNSServiceRefWrap(ref, DNSServiceBrowser);
		obj->Wrap(args.This());
		refMap[ref] = Local<Object>::New(isolate, args.This());

		/*
		std::thread fdLoop([](DNSServiceRefWrap *obj){
			int dns_fd = DNSServiceRefSockFD(*obj->ref);
			fd_set readfds;
			while(obj->ref) {
				FD_ZERO(&readfds);
				FD_SET(dns_fd, &readfds);
				if (select(dns_fd + 1, &readfds, NULL, NULL, NULL) > 0) {
					obj->refLock.lock();
					if (obj->ref)
						DNSServiceProcessResult(*obj->ref);
					obj->refLock.unlock();
				}
			}
		}, obj);
		*/

		args.This()->Set(String::NewSymbol("service"), args[0]->ToString(), ReadOnly);
		args.This()->Set(String::NewSymbol("listening"), True(isolate), ReadOnly);
		args.This()->Set(String::NewSymbol("sockFd"), Integer::New(DNSServiceRefSockFD(*obj->ref)), ReadOnly);
		NODE_SET_METHOD(args.This(), "processResult", DNSServiceRefWrap::DNSServiceDoProcessResult);
		NODE_SET_METHOD(args.This(), "removeInits", DNSServiceRefWrap::RemoveBrowserInits);
		args.GetReturnValue().Set(args.This());
	} else {
		const int argc = 1;
		Local<Value> argv[argc] = { args[0] };
		Local<Function> cons = Local<Function>::New(isolate, BrowserConstructor);
		args.GetReturnValue().Set(cons->NewInstance(argc, argv));
	}
}

void DNSServiceRefWrap::DNSServiceBrowseReply(DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *serviceName, const char *regtype, const char *replyDomain, void *context) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	ThrowException(String::NewFromUtf8(isolate, "Got something"));

	Local<Value> argv[1] = { True(isolate) };
	MakeCallback(refMap[&sdRef], "emit", 1, argv);
}

void DNSServiceRefWrap::DNSServiceDoProcessResult(const FunctionCallbackInfo<Value> &args) {
	Isolate* isolate = Isolate::GetCurrent();
	DNSServiceRefWrap* obj = ObjectWrap::Unwrap<DNSServiceRefWrap>(args.Holder());
	if (obj->ref) {
		DNSServiceProcessResult(*obj->ref);
		args.GetReturnValue().Set(args.Holder());
	} else {
		ThrowException(Exception::ReferenceError(String::NewFromUtf8(isolate, "Invoked processResult on terminated browser")));
	}
}

void DNSServiceRefWrap::RemoveBrowserInits(const FunctionCallbackInfo<Value> &args) {
	Isolate* isolate = Isolate::GetCurrent();
	Local<Object> This = args.Holder();
	if (This->Has(String::NewSymbol("removeInits"))) {
		This->ForceDelete(String::NewSymbol("sockFd"));
		This->ForceDelete(String::NewSymbol("processResult"));
		This->ForceDelete(String::NewSymbol("removeInits"));
		args.GetReturnValue().Set(args.Holder());
	} else {
		ThrowException(Exception::ReferenceError(String::NewFromUtf8(isolate, "Inits already removed")));
	}
}

void DNSServiceRefWrap::Terminate(const FunctionCallbackInfo<Value> &args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	DNSServiceRefWrap* obj = ObjectWrap::Unwrap<DNSServiceRefWrap>(args.Holder());
	if (obj->ref) {
		obj->refLock.lock();
		DNSServiceRefDeallocate(*obj->ref);
		free(obj->ref);
		obj->ref = NULL;
		obj->refLock.unlock();

		if (obj->type == DNSServiceAdvertisement)
			args.Holder()->ForceSet(String::NewSymbol("advertising"), False(isolate), ReadOnly);
		else if (obj->type == DNSServiceBrowser)
			args.Holder()->ForceSet(String::NewSymbol("listening"), False(isolate), ReadOnly);
		args.GetReturnValue().Set(args.Holder());
	} else {
		if (obj->type == DNSServiceAdvertisement)
			ThrowException(Exception::ReferenceError(String::NewFromUtf8(isolate, "Advertisement already terminated")));
		else if (obj->type == DNSServiceBrowser)
			ThrowException(Exception::ReferenceError(String::NewFromUtf8(isolate, "Browser already terminated")));
	}
}

