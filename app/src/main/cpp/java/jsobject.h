#ifndef _jsobject_h_
#define _jsobject_h_

#include <jni.h>
#include <android/log.h>
#include <stdlib.h>

#include "v8.h"
#include "node.h"
#include "node_object_wrap.h"

#include "../utils/utils.h"

namespace node {

    using v8::Value;
    using v8::Local;
    using v8::Handle;
    using v8::Object;
    using v8::Isolate;
    using v8::Persistent;
    using v8::FunctionTemplate;
    using v8::FunctionCallbackInfo;

    namespace jvm {

        class JSObject : public ObjectWrap {
        public:
            JSObject(jobject, jmethodID, jlong);
            virtual ~JSObject();
            static void Init(Isolate *isolate);
            static void New(const FunctionCallbackInfo<Value> &args);
            static void Call(const FunctionCallbackInfo<Value> &args);
            static Local<Value> NewInstance(Isolate *, jobject, jmethodID, jlong);

        public:
            static Persistent<FunctionTemplate> _func_wrapper;

        private:
            jobject _observer;
            jmethodID _subscribe;
            jlong _runtimePtr;
        };

    }  // anonymous namespace

} // namespace node

#endif // _jsobject_h_
