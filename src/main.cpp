
#include <napi.h>

#include "include/loader.h"

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    Napi::Value a = exports;
    load_instance_controller(env, exports);
    load_instance_instance(env, exports);
    load_instance_resource(env, exports);
    load_utility_buffer(env, exports);
    load_utility_utility(env, exports);
    load_device_device(env, exports);
    load_win32_window(env, exports);
    return exports;
}

NODE_API_MODULE(maa, Init)
