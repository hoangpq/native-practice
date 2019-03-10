#ifndef _v8context_h_
#define _v8context_h_

#include <android/log.h>
#include <env-inl.h>
#include <env.h>
#include <iostream>
#include <jni.h>
#include <node.h>
#include <uv.h>
#include <v8.h>

#include "jsobject.h"
#include "../utils/utils.h"

extern "C" jobject createTimeoutHandler(JNIEnv **);
extern "C" void postDelayed(JNIEnv **, jobject, jlong, jlong, jint);

namespace node {

namespace av8 {

static JNIEnv *env_ = nullptr;

class V8Runtime {
public:
  Isolate *isolate_;
  Persistent<Context> context_;
};

} // namespace av8
} // namespace node

#endif
