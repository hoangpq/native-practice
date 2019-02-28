#include <v8.h>
#include "jsobject.h"
#include "../java/java.h"
#include "v8context.h"

namespace node {

    using v8::Local;
    using v8::Value;
    using v8::Number;
    using v8::Handle;
    using v8::String;
    using v8::Isolate;
    using v8::Boolean;
    using v8::Persistent;
    using v8::Undefined;
    using v8::Exception;
    using v8::HandleScope;
    using v8::ObjectTemplate;
    using v8::FunctionTemplate;
    using v8::EscapableHandleScope;
    using v8::FunctionCallbackInfo;

    namespace jvm {

        using util::Util;

        Persistent<FunctionTemplate> JSObject::constructor_;

        JSObject::JSObject(jclass c) : class_(c) {};

        JSObject::JSObject(jclass c, string method) : class_(c), method_(method) {};

        JSObject::~JSObject() = default;

        void JSObject::Init(Isolate *isolate) {
            Local<FunctionTemplate> ft_ = FunctionTemplate::New(isolate, New);
            Local<ObjectTemplate> it_ = ft_->InstanceTemplate();
            it_->SetInternalFieldCount(1);
            it_->SetNamedPropertyHandler(NamedGetter);
            it_->SetCallAsFunctionHandler(Call, Handle<Value>());
            constructor_.Reset(isolate, ft_);
        }

        void JSObject::New(const FunctionCallbackInfo<Value> &args) {
            Isolate *isolate = args.GetIsolate();
            if (args.IsConstructCall()) {
                args.GetReturnValue().Set(args.This());
            } else {
                isolate->ThrowException(
                        String::NewFromUtf8(isolate, "Function is not constructor."));
            }
        }

        Handle<Object>
        JSObject::NewInstance(Isolate *isolate_, jclass class_, string method_) {
            Handle<FunctionTemplate> _function_template =
                    Local<FunctionTemplate>::New(isolate_, constructor_);

            Local<Object> instance_ = _function_template->GetFunction()->NewInstance();

            auto *wrapper = !method_.empty()
                            ? new JSObject(class_, method_)
                            : new JSObject(class_);

            wrapper->Wrap(instance_);
            return instance_;
        }

        void JSObject::NamedGetter(Local<String> key, const PropertyCallbackInfo<Value> &info) {
            Isolate *isolate_ = info.GetIsolate();
            String::Utf8Value m(key->ToString());
            string method_(*m);

            auto *parent = ObjectWrap::Unwrap<JSObject>(info.Holder());
            jclass class_ = parent->GetObjectClass();

            Handle<Object> instance_ = NewInstance(isolate_, class_, method_);
            info.GetReturnValue().Set(instance_);
        }

        void JSObject::Call(const FunctionCallbackInfo<Value> &args) {
            Isolate *isolate_ = args.GetIsolate();

            JNIEnv *env = nullptr;
            JavaType::InitEnvironment(isolate_, &env);

            jclass utilClass = env->FindClass("com/node/util/JNIUtils");
            jmethodID getPackageName = env->GetStaticMethodID(
                    utilClass, "getPackageName", "(Ljava/lang/Class;)Ljava/lang/String;");

            auto *parent = ObjectWrap::Unwrap<JSObject>(args.Holder());

            auto packageName = (jstring) env->CallStaticObjectMethod(
                    utilClass, getPackageName, parent->GetObjectClass());

            auto packageName_ = Util::JavaToString(env, packageName);

            args.GetReturnValue().Set(Util::ConvertToV8String(packageName_));
        }

    }
}
