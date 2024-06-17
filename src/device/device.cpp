#include "../include/helper.h"
#include "../include/loader.h"

#include <MaaFramework/MaaAPI.h>
#include <MaaToolkit/MaaToolkitAPI.h>
#include <napi.h>

Napi::Value post_find_device(const Napi::CallbackInfo& info)
{
    return Napi::Boolean::New(info.Env(), MaaToolkitPostFindDevice() > 0);
}

Napi::Value post_find_device_with_adb(const Napi::CallbackInfo& info)
{
    Napi::String adb = info[0].As<Napi::String>();
    return Napi::Boolean::New(info.Env(), MaaToolkitPostFindDevice() > 0);
}

Napi::Value is_find_device_completed(const Napi::CallbackInfo& info)
{
    return Napi::Boolean::New(info.Env(), MaaToolkitIsFindDeviceCompleted() > 0);
}

class WaitForFindDeviceToCompleteWork : public Napi::AsyncWorker
{
public:
    WaitForFindDeviceToCompleteWork(const Napi::Function& cb)
        : AsyncWorker(cb)
    {
    }

    void Execute() override { size = MaaToolkitWaitForFindDeviceToComplete(); }

    void OnOK() override
    {
        auto env = Env();
        Callback().Call({ env.Undefined(), Napi::Number::New(env, size) });
    }

    void OnError(const Napi::Error& e) override
    {
        auto env = Env();
        Callback().Call({ e.Value(), env.Undefined() });
    }

private:
    MaaSize size = 0;
};

Napi::Value wait_for_find_device_to_complete(const Napi::CallbackInfo& info)
{
    Napi::Function cb = info[0].As<Napi::Function>();
    auto worker = new SimpleAsyncWork<MaaSize>(
        cb,
        MaaToolkitWaitForFindDeviceToComplete,
        [](auto env, auto res) { return Napi::Number::New(env, res); });
    worker->Queue();
    return info.Env().Undefined();
}

Napi::Value get_device_count(const Napi::CallbackInfo& info)
{
    return Napi::Number::New(info.Env(), MaaToolkitGetDeviceCount());
}

Napi::Value get_device(const Napi::CallbackInfo& info)
{
    auto index = info[0].ToNumber().Uint32Value();
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
    exports["post_find_device"] =
        Napi::Function::New(env, post_find_device, "MaaToolkitPostFindDevice");
    exports["post_find_device_with_adb"] =
        Napi::Function::New(env, post_find_device_with_adb, "MaaToolkitPostFindDeviceWithAdb");
    exports["is_find_device_completed"] =
        Napi::Function::New(env, is_find_device_completed, "MaaToolkitIsFindDeviceCompleted");
    exports["wait_for_find_device_to_complete"] = Napi::Function::New(
        env,
        wait_for_find_device_to_complete,
        "MaaToolkitWaitForFindDeviceToComplete");
    exports["get_device_count"] =
        Napi::Function::New(env, get_device_count, "MaaToolkitGetDeviceCount");
    exports["get_device"] = Napi::Function::New(env, get_device, "MaaToolkitGetDeviceXXX");
}
