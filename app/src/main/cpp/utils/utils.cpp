#include "utils.h"
#include <jni.h>
#include <string>

namespace util {

string Util::JavaToString(JNIEnv *env, jstring str) {
  jclass objClazz = env->GetObjectClass(str);
  jmethodID methodId =
      env->GetMethodID(objClazz, "getBytes", "(Ljava/lang/String;)[B");

  jstring charsetName = env->NewStringUTF("UTF-8");
  auto byteArray =
      (jbyteArray)env->CallObjectMethod(str, methodId, charsetName);
  env->DeleteLocalRef(charsetName);

  jbyte *pBytes = env->GetByteArrayElements(byteArray, nullptr);

  const jsize length = env->GetArrayLength(byteArray);
  std::string results((const char *)pBytes, (unsigned long)length);

  env->ReleaseByteArrayElements(byteArray, pBytes, JNI_ABORT);
  env->DeleteLocalRef(byteArray);

  return results;
}

Local<String> Util::ConvertToV8String(const string &s) {
  auto isolate = Isolate::GetCurrent();
  return String::NewFromUtf8(isolate, s.c_str());
}

void Util::InitEnvironment(Isolate *isolate, JNIEnv **env) {
  jint res =
      g_ctx.javaVM->GetEnv(reinterpret_cast<void **>(&(*env)), JNI_VERSION_1_6);
  if (res != JNI_OK) {
    res = g_ctx.javaVM->AttachCurrentThread(&(*env), nullptr);
    if (JNI_OK != res) {
      isolate->ThrowException(
          Util::ConvertToV8String("Unable to invoke activity!"));
    }
  }
}

void Util::AttachCurrentThread(JNIEnv **env) {
  int res =
      g_ctx.javaVM->GetEnv(reinterpret_cast<void **>(&(*env)), JNI_VERSION_1_6);
  if (res != JNI_OK) {
    res = g_ctx.javaVM->AttachCurrentThread(&(*env), nullptr);
    if (JNI_OK != res) {
      return;
    }
  }
}

} // namespace util
