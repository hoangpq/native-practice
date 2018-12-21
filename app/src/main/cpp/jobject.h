#ifndef _jobject_h_
#define _jobject_h_

#include <jni.h>
#include <android/log.h>

#include "v8.h"
#include "node.h"
#include "node_object_wrap.h"

#include "context.h"

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

        class JavaFunctionWrapper : public ObjectWrap {
        public:
            JavaFunctionWrapper(jobject, jmethodID, char *);
            virtual ~JavaFunctionWrapper();
            static void Init(Isolate *isolate);
            static void New(const FunctionCallbackInfo<Value> &args);
            static void Call(const FunctionCallbackInfo<Value> &args);
            static Local<Value> NewInstance(Isolate *, jobject, jmethodID, char *);

        public:
            static Persistent<FunctionTemplate> _func_wrapper;

        private:
            jobject _instance;
            jmethodID _methodId;
            char *_methodName;
        };

    }  // anonymous namespace

} // namespace node

#endif // _jobject_h_
