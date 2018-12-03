#include <v8.h>
#include "java.h"
#include "node-ext.h"
#include "native-lib.h"

namespace node {

    using v8::Context;
    using v8::Local;
    using v8::Number;
    using v8::String;
    using v8::Object;
    using v8::Value;
    using v8::Isolate;
    using v8::Function;
    using v8::Exception;
    using v8::HandleScope;
    using v8::ObjectTemplate;
    using v8::FunctionTemplate;
    using v8::EscapableHandleScope;
    using v8::FunctionCallbackInfo;
    using v8::MaybeLocal;
    using v8::JSON;
    using node::jvm::JavaType;

    namespace loader {

        const char *ToCString(Local<String> str) {
            String::Utf8Value value(str);
            return *value ? *value : "<string conversion failed>";
        }

        void AndroidToast(const FunctionCallbackInfo<Value> &args) {
            JNIEnv *env;
            JavaType::InitEnvironment(args, &env);
            Local<String> str = args[0]->ToString();
            const char *msg = ToCString(str);

            jmethodID methodId = env->GetMethodID(
                    g_ctx.mainActivityClz, "subscribe", "(Ljava/lang/String;)V");

            jstring javaMsg = env->NewStringUTF(msg);
            env->CallVoidMethod(g_ctx.mainActivityObj, methodId, javaMsg);
            env->DeleteLocalRef(javaMsg);
            args.GetReturnValue().Set(str);
        }

        void AndroidLog(const FunctionCallbackInfo<Value> &args) {
            Isolate *isolate = args.GetIsolate();
            Local<Context> context = isolate->GetCurrentContext();

            EscapableHandleScope handle_scope(isolate);
            Local<String> result =
                    handle_scope.Escape(
                            JSON::Stringify(context, args[0]->ToObject()).ToLocalChecked());
            const char *jsonString = ToCString(result);
            LOGI("%s", jsonString);
        }

        void AndroidError(const FunctionCallbackInfo<Value> &args) {
            Isolate *isolate = args.GetIsolate();
            Local<Context> context = isolate->GetCurrentContext();

            EscapableHandleScope handle_scope(isolate);
            Local<String> result =
                    handle_scope.Escape(
                            JSON::Stringify(context, args[0]->ToObject()).ToLocalChecked());
            const char *jsonString = ToCString(result);
            LOGE("%s", jsonString);
        }

        void JVMOnLoad(const FunctionCallbackInfo<Value> &args) {
            Isolate *isolate = args.GetIsolate();
            if (args[0]->IsFunction()) {
                Local<Object> context = Object::New(isolate);
                Local<Function> onJvmCreatedFunc = args[0].As<Function>();
                if (jvmInitialized) {
                    onJvmCreatedFunc->Call(context, 0, NULL);
                }
            }
        }

        // Override header
        class ModuleWrap {
        public:
            static void Initialize(v8::Local<v8::Object> target,
                                   v8::Local<v8::Value> unused,
                                   v8::Local<v8::Context> context) {};
        };

        class AndroidModuleWrap : public ModuleWrap {
        public:
            static void New(const FunctionCallbackInfo<Value> &args) {
                Isolate *isolate = args.GetIsolate();
                if (args.IsConstructCall()) {
                    JavaType *jvm = new JavaType(&g_ctx.javaVM);
                    jvm->PWrap(args.This());
                    args.GetReturnValue().Set(args.This());
                } else {
                    isolate->ThrowException(
                            String::NewFromUtf8(isolate, "Function is not constructor."));
                }
            }

            static void Initialize(Local<Object> target,
                                   Local<Value> unused,
                                   Local<Context> context,
                                   void *priv) {

                ModuleWrap::Initialize(target, unused, context);
                // define function in global context
                v8::Isolate *isolate = target->GetIsolate();
                Local<Object> global = context->Global();

                auto toastFn = FunctionTemplate::New(isolate, loader::AndroidToast)->GetFunction();
                global->Set(String::NewFromUtf8(isolate, "$toast"), toastFn);

                auto logFn = FunctionTemplate::New(isolate, loader::AndroidLog)->GetFunction();
                global->Set(String::NewFromUtf8(isolate, "$log"), logFn);

                auto errFn = FunctionTemplate::New(isolate, loader::AndroidError)->GetFunction();
                global->Set(String::NewFromUtf8(isolate, "$error"), errFn);

                auto onLoadFn = FunctionTemplate::New(isolate, loader::JVMOnLoad)->GetFunction();
                global->Set(String::NewFromUtf8(isolate, "$load"), onLoadFn);

                static v8::Persistent<v8::Function> constructor;
                Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
                tpl->SetClassName(String::NewFromUtf8(isolate, "Java"));
                tpl->InstanceTemplate()->SetInternalFieldCount(1);
                constructor.Reset(isolate, tpl->GetFunction());

                Local<Function> cons = Local<Function>::New(isolate, constructor);
                Local<Object> instance = cons->NewInstance(context).ToLocalChecked();

                Local<String> $vm = String::NewFromUtf8(isolate, "$vm");
                global->Set($vm, instance);
            }

        };

    }

}

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *) {
    JNIEnv *env;
    memset(&g_ctx, 0, sizeof(g_ctx));
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR; // JNI version not supported.
    }
    g_ctx.javaVM = vm;
    g_ctx.mainActivityObj = NULL;
    jvmInitialized = true;
    return JNI_VERSION_1_6;
}

JNIEXPORT void JNI_OnUnload(JavaVM *vm, void *) {
    JNIEnv *env;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_EDETACHED) {
        vm->DetachCurrentThread();
    }
}

extern "C" void JNICALL
Java_com_node_sample_MainActivity_initVM(
        JNIEnv *env,
        jobject klass,
        jobject callback) {

    // init objects
    jclass clz = env->GetObjectClass(callback);
    g_ctx.mainActivityClz = (jclass) env->NewGlobalRef(clz);
    g_ctx.mainActivityObj = env->NewGlobalRef(callback);
}

extern "C" void JNICALL
Java_com_node_sample_MainActivity_releaseVM(
        JNIEnv *env,
        jobject /* this */) {

    // release allocated objects
    env->DeleteGlobalRef(g_ctx.mainActivityObj);
    env->DeleteGlobalRef(g_ctx.mainActivityClz);
    g_ctx.mainActivityObj = NULL;
    g_ctx.mainActivityClz = NULL;
}

NODE_MODULE_CONTEXT_AWARE_BUILTIN(module_wrap, node::loader::AndroidModuleWrap::Initialize);
