#ifndef _context_h
#define _context_h

#include <iostream>
#include <jni.h>
#include <string>
#include <v8.h>

using namespace std;
using namespace v8;

typedef struct NodeContext {
  JavaVM *javaVM;
  JNIEnv *env;
  jclass mainActivityClz;
  jobject mainActivityObj;
  jobject mainActivity;
  Isolate *isolate_;
} NodeContext;

namespace util {

class Util {
public:
  static string JavaToString(JNIEnv *env, jstring str);
  static Local<String> ConvertToV8String(const string &s);
  static string GetPackageName(JNIEnv *env, jclass class_);
  static void InitEnvironment(Isolate *isolate, JNIEnv **env);
};
} // namespace util

extern NodeContext g_ctx;

static const char *kTAG = "V8 Runtime";

#define LOGD(...)                                                              \
  ((void)__android_log_print(ANDROID_LOG_DEBUG, kTAG, __VA_ARGS__))

#define LOGE(...)                                                              \
  ((void)__android_log_print(ANDROID_LOG_ERROR, kTAG, __VA_ARGS__))

#endif
