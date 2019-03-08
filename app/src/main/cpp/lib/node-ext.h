#ifndef _node_extension_h_
#define _node_extension_h_

#include <jni.h>
#include <cstddef>
#include <cstdint>
#include <string>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#include <android/log.h>

#include "v8.h"
#include "env.h"
#include "env-inl.h"
#include "node_buffer.h"
#include "node.h"
#include "../utils/utils.h"
#include "../java/java.h"
#include "../java/jobject.h"

extern "C" jlong JNICALL Java_com_node_sample_MainActivity_createPointer(JNIEnv *, jobject);
extern "C" jstring JNICALL Java_com_node_sample_MainActivity_getUtf8String(JNIEnv *, jobject);
extern "C" void onNodeServerLoaded(JNIEnv **, jobject);
extern "C" jobject createTimeoutHandler(JNIEnv **);

namespace node {

    namespace loader {
        void AndroidToast(const FunctionCallbackInfo<Value> &args);
        void AndroidLog(const FunctionCallbackInfo<Value> &args);
        void AndroidError(const FunctionCallbackInfo<Value> &args);
        void OnLoad(const FunctionCallbackInfo<Value> &args);
    }

}

#endif
