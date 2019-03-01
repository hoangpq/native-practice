#ifndef _jsobject_h_
#define _jsobject_h_

#include <jni.h>
#include <android/log.h>
#include <cstdlib>

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
            explicit JSObject(jclass);
            ~JSObject() override;

            static void Init(Isolate *isolate);
            static void New(const FunctionCallbackInfo<Value> &args);
            static void Call(const FunctionCallbackInfo<Value> &args);
            static Handle<Object> NewInstance(Isolate*, jclass, string method_ = "");

        public:
            static Persistent<FunctionTemplate> constructor_;
            static void NamedGetter(Local<String>, const PropertyCallbackInfo<Value> &);

        public: // getters
            jclass GetObjectClass() { return class_; }

        private:
            jclass class_;
            string type_;
            JNIEnv *env_;

        };

    }

}

#endif
