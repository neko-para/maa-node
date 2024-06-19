#include "../include/helper.h"
#include "../include/loader.h"

#include <MaaFramework/MaaAPI.h>
#include <MaaToolkit/MaaToolkitAPI.h>
#include <napi.h>

Napi::Value init_option_config(const Napi::CallbackInfo& info)
{
    auto user_path = info[0].As<Napi::String>().Utf8Value();
    auto default_json = info[1].As<Napi::String>().Utf8Value();
    return Napi::Boolean::New(
        info.Env(),
        MaaToolkitInitOptionConfig(user_path.c_str(), default_json.c_str()));
}

void load_config_config(Napi::Env env, Napi::Object& exports)
{
    BIND(init_option_config);
}
