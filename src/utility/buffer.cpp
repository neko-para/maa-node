#include "utility.h"

#include <MaaFramework/MaaAPI.h>
#include <napi.h>

void ImageBufferFinalizer(Napi::Env, MaaImageBufferHandle handle) {
  MaaDestroyImageBuffer(handle);
}

Napi::Value create_image_buffer(const Napi::CallbackInfo &info) {
  if (info.Length() != 0) {
    throw Napi::TypeError::New(info.Env(), "create_image_buffer expect []");
  }
  return Napi::External<MaaImageBuffer>::New(info.Env(), MaaCreateImageBuffer(),
                                             ImageBufferFinalizer);
}

Napi::Value is_image_empty(const Napi::CallbackInfo &info) {
  if (info.Length() != 1) {
    throw Napi::TypeError::New(info.Env(),
                               "is_image_empty expect [ImageBufferHandle]");
  }
  auto handle = info[0].As<Napi::External<MaaImageBuffer>>();
  if (!handle) {
    throw Napi::TypeError::New(info.Env(),
                               "is_image_empty expect [ImageBufferHandle]");
  }
  return Napi::Boolean::New(info.Env(), MaaIsImageEmpty(handle.Data()) > 0);
}

void load_utility_buffer(Napi::Env env, Napi::Object &exports) {
  exports["create_image_buffer"] =
      Napi::Function::New(env, create_image_buffer, "MaaCreateImageBuffer");
  exports["is_image_empty"] =
      Napi::Function::New(env, is_image_empty, "MaaIsImageEmpty");
}
