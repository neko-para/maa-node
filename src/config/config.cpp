#include "../include/helper.h"
#include "../include/loader.h"

#include <MaaFramework/MaaAPI.h>
#include <MaaToolkit/MaaToolkitAPI.h>
#include <napi.h>

Napi::Value init_option_config(const Napi::CallbackInfo& info)
{
    CheckCount(info, 2);
    auto user_path = CheckAsString(info[0]);
    auto default_json = CheckAsString(info[1]);
    return Napi::Boolean::New(
        info.Env(),
        MaaToolkitInitOptionConfig(user_path.c_str(), default_json.c_str()));
}

void load_config_config(Napi::Env env, Napi::Object& exports)
{
    BIND(init_option_config);
}
