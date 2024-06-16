
#include <napi.h>

#include "utility/utility.h"

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  Napi::Value a = exports;
  load_utility(env, exports);
  return exports;
}

NODE_API_MODULE(maa, Init)