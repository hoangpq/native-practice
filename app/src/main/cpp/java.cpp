#include "java.h"

namespace node {

    using v8::Context;
    using v8::Function;
    using v8::FunctionCallbackInfo;
    using v8::FunctionTemplate;
    using v8::Isolate;
    using v8::Handle;
    using v8::Local;
    using v8::Number;
    using v8::Object;
    using v8::Persistent;
    using v8::String;
    using v8::Value;

    namespace jvm {

        Persistent<Function> JavaType::constructor;

        JavaType::JavaType(JavaVM *vm) : _jvm(vm) {}

        JavaType::~JavaType() {}

        void JavaType::Init(Isolate *isolate) {
            // Prepare constructor template
            Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
            tpl->SetClassName(String::NewFromUtf8(isolate, "Java"));
            tpl->InstanceTemplate()->SetInternalFieldCount(1);
            // Prototype
            NODE_SET_PROTOTYPE_METHOD(tpl, "$toast", Toast);
            Local<Context> context = isolate->GetCurrentContext();
            constructor.Reset(isolate, tpl->GetFunction(context).ToLocalChecked());
        }

        void JavaType::New(const FunctionCallbackInfo<Value> &args) {
            Isolate *isolate = args.GetIsolate();
            if (args.IsConstructCall()) {
                args.GetReturnValue().Set(args.This());
            } else {
                isolate->ThrowException(
                        String::NewFromUtf8(isolate, "Function is not constructor."));
            }
        }

        void JavaType::NewInstance(const FunctionCallbackInfo<Value> &args) {
            Isolate *isolate = args.GetIsolate();

            const unsigned argc = 1;
            Local<Value> argv[argc] = {args[0]};
            Local<Function> cons = Local<Function>::New(isolate, constructor);
            Local<Context> context = isolate->GetCurrentContext();
            Local<Object> instance =
                    cons->NewInstance(context, argc, argv).ToLocalChecked();

            args.GetReturnValue().Set(instance);
        }

        void JavaType::Toast(const v8::FunctionCallbackInfo<v8::Value> &args) {
            Isolate *isolate = args.GetIsolate();
            Handle <Context> context = isolate->GetCurrentContext();
            Local<String> fnName = String::NewFromUtf8(isolate, "$toast");
            Handle<Object> global = context->Global();
            // Get $toast function from global context
            Local<Function> $toast = Local<Function>::Cast(global->Get(context, fnName).ToLocalChecked());
            Local<Value> funcArgs[1];
            funcArgs[0] = String::NewFromUtf8(isolate, "Invoke $toast function in global context successfully!");
            $toast->Call(global, 1, funcArgs);
        }

        void CreateJavaType(const FunctionCallbackInfo<Value> &args) {
            jvm::JavaType::NewInstance(args);
        }

        void InitAll(Local<Object> target) {
            JavaType::Init(target->GetIsolate());
            NODE_SET_METHOD(target, "type", CreateJavaType);
        }

        void InitJavaVM(Local<Object> target) {
            InitAll(target);
        }

    }  // anonymous namespace


} // namespace node

NODE_MODULE_CONTEXT_AWARE_BUILTIN(java, node::jvm::InitJavaVM);
