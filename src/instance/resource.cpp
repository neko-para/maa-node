#include "../include/helper.h"
#include "../include/info.h"
#include "../include/loader.h"

#include <MaaFramework/MaaAPI.h>

Napi::Value resource_create(const Napi::CallbackInfo& info)
{
    CheckCount(info, 1);
    MaaResourceCallback cb = nullptr;
    CallbackContext* ctx = nullptr;
    MaaResourceHandle handle = nullptr;

    if (!info[5].IsNull()) {
        auto callback = CheckAsFunction(info[0]);

        cb = TrivialCallback;
        ctx = new CallbackContext { info.Env(), callback, "TrivialCallback" };
    }

    handle = MaaResourceCreate(cb, ctx);

    if (handle) {
        return Napi::External<ResourceInfo>::New(
            info.Env(),
            new ResourceInfo { handle, ctx },
            &DeleteFinalizer<ResourceInfo*>);
    }
    else {
        delete ctx;
        return info.Env().Null();
    }
}

Napi::Value resource_destroy(const Napi::CallbackInfo& info)
{
    CheckCount(info, 1);
    auto handle = ResourceInfo::FromValue(info[0]);
    handle->dispose();
    return info.Env().Undefined();
}

Napi::Value resource_post_path(const Napi::CallbackInfo& info)
{
    CheckCount(info, 2);
    auto handle = ResourceInfo::HandleFromValue(info[0]);
    auto path = CheckAsString(info[1]);
    auto resid = MaaResourcePostPath(handle, path.c_str());
    return Napi::Number::New(info.Env(), resid);
}

Napi::Value resource_clear(const Napi::CallbackInfo& info)
{
    CheckCount(info, 1);
    auto handle = ResourceInfo::HandleFromValue(info[0]);
    return Napi::Boolean::New(info.Env(), MaaResourceClear(handle));
}

Napi::Value resource_status(const Napi::CallbackInfo& info)
{
    CheckCount(info, 2);
    auto handle = ResourceInfo::HandleFromValue(info[0]);
    auto id = CheckAsNumber(info[1]).Uint32Value();
    return Napi::Number::New(info.Env(), MaaResourceStatus(handle, id));
}

Napi::Value resource_wait(const Napi::CallbackInfo& info)
{
    CheckCount(info, 2);
    auto handle = ResourceInfo::HandleFromValue(info[0]);
    auto id = CheckAsNumber(info[1]).Uint32Value();
    auto worker = new SimpleAsyncWork<MaaStatus>(
        info.Env(),
        [handle, id]() { return MaaResourceWait(handle, id); },
        [](auto env, auto res) { return Napi::Number::New(env, res); });
    worker->Queue();
    return worker->Promise();
}

Napi::Value resource_loaded(const Napi::CallbackInfo& info)
{
    CheckCount(info, 1);
    auto handle = ResourceInfo::HandleFromValue(info[0]);
    return Napi::Boolean::New(info.Env(), MaaResourceLoaded(handle));
}

Napi::Value resource_get_hash(const Napi::CallbackInfo& info)
{
    CheckCount(info, 1);
    auto handle = ResourceInfo::HandleFromValue(info[0]);
    StringBuffer buffer;
    auto ret = MaaResourceGetHash(handle, buffer);
    if (ret) {
        return Napi::String::New(info.Env(), buffer);
    }
    else {
        return info.Env().Null();
    }
}

Napi::Value resource_get_task_list(const Napi::CallbackInfo& info)
{
    CheckCount(info, 1);
    auto handle = ResourceInfo::HandleFromValue(info[0]);
    StringBuffer buffer;
    auto ret = MaaResourceGetTaskList(handle, buffer);
    if (ret) {
        return Napi::String::New(info.Env(), buffer);
    }
    else {
        return info.Env().Null();
    }
}

void load_instance_resource(Napi::Env env, Napi::Object& exports)
{
    BIND(resource_create);
    BIND(resource_destroy);
    BIND(resource_post_path);
    BIND(resource_clear);
    BIND(resource_status);
    BIND(resource_wait);
    BIND(resource_loaded);
    BIND(resource_get_hash);
    BIND(resource_get_task_list);
}
