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
    auto worker = new WaitForFindDeviceToCompleteWork(cb);
    worker->Queue();
    return info.Env().Undefined();
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
}
