#include <MaaFramework/MaaAPI.h>
#include <napi.h>

#include "utility.h"

Napi::Value version(const Napi::CallbackInfo &info) {
  return Napi::String::New(info.Env(), MaaVersion());
}

void load_utility(Napi::Env env, Napi::Object &exports) {
  exports["version"] = Napi::Function::New(env, version, "MaaVersion");
}
