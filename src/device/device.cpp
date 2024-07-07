#include "../include/helper.h"
#include "../include/loader.h"

#include <MaaFramework/MaaAPI.h>
#include <MaaToolkit/MaaToolkitAPI.h>
#include <napi.h>

Napi::Value post_find_device(const Napi::CallbackInfo& info)
{
    CheckCount(info, 0);
    return Napi::Boolean::New(info.Env(), MaaToolkitPostFindDevice());
}

Napi::Value post_find_device_with_adb(const Napi::CallbackInfo& info)
{
    CheckCount(info, 1);
    auto adb = CheckAsString(info[0]);
    return Napi::Boolean::New(info.Env(), MaaToolkitPostFindDeviceWithAdb(adb.c_str()));
}

Napi::Value is_find_device_completed(const Napi::CallbackInfo& info)
{
    CheckCount(info, 0);
    return Napi::Boolean::New(info.Env(), MaaToolkitIsFindDeviceCompleted());
}

Napi::Value wait_for_find_device_to_complete(const Napi::CallbackInfo& info)
{
    CheckCount(info, 0);
    auto worker = new SimpleAsyncWork<MaaSize>(
        info.Env(),
        MaaToolkitWaitForFindDeviceToComplete,
        [](auto env, auto res) { return Napi::Number::New(env, res); });
    worker->Queue();
    return worker->Promise();
}

Napi::Value get_device_count(const Napi::CallbackInfo& info)
{
    CheckCount(info, 0);
    return Napi::Number::New(info.Env(), MaaToolkitGetDeviceCount());
}

Napi::Value get_device(const Napi::CallbackInfo& info)
{
    CheckCount(info, 1);
    auto index = CheckAsNumber(info[0]).Uint32Value();
    auto result = Napi::Object::New(info.Env());
    result["name"] = MaaToolkitGetDeviceName(index);
    result["adb_path"] = MaaToolkitGetDeviceAdbPath(index);
    result["adb_serial"] = MaaToolkitGetDeviceAdbSerial(index);
    result["adb_controller_type"] = MaaToolkitGetDeviceAdbControllerType(index);
    result["adb_config"] = MaaToolkitGetDeviceAdbConfig(index);
    return result;
}

void load_device_device(Napi::Env env, Napi::Object& exports)
{
    BIND(post_find_device);
    BIND(post_find_device_with_adb);
    BIND(is_find_device_completed);
    BIND(wait_for_find_device_to_complete);
    BIND(get_device_count);
    BIND(get_device);
}
