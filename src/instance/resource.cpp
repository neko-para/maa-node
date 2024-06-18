#include "../include/helper.h"
#include "../include/loader.h"

#include <MaaFramework/MaaAPI.h>

#include <iostream>

void ResourceFinalzer(Napi::Env env, MaaResourceHandle handle, void* hint)
{
    std::cerr << "destroy resource" << std::endl;
    MaaResourceDestroy(handle);
    if (hint) {
        auto ctx = reinterpret_cast<CallbackContext*>(hint);
        delete ctx;
    }
}

Napi::Value resource_create(const Napi::CallbackInfo& info)
{
    MaaResourceCallback cb = nullptr;
    CallbackContext* ctx = nullptr;
    MaaResourceHandle handle = nullptr;

    if (!info[5].IsNull()) {
        auto callback = info[0].As<Napi::Function>();

        cb = TrivialCallback;
        ctx = new CallbackContext { info.Env(), callback, "TrivialCallback" };
    }

    handle = MaaResourceCreate(cb, ctx);

    if (handle) {
        return Napi::External<MaaResourceAPI>::New(info.Env(), handle, ResourceFinalzer, ctx);
    }
    else {
        delete ctx;
        return info.Env().Null();
    }
}

Napi::Value resource_post_path(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaResourceAPI>>().Data();
    auto path = info[1].As<Napi::String>().Utf8Value();
    auto resid = MaaResourcePostPath(handle, path.c_str());
    return Napi::Number::New(info.Env(), resid);
}

Napi::Value resource_clear(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaResourceAPI>>().Data();
    return Napi::Boolean::New(info.Env(), MaaResourceClear(handle));
}

Napi::Value resource_status(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaResourceAPI>>().Data();
    auto id = info[1].As<Napi::Number>().Uint32Value();
    return Napi::Number::New(info.Env(), MaaResourceStatus(handle, id));
}

Napi::Value resource_wait(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaResourceAPI>>().Data();
    auto id = info[1].As<Napi::Number>().Uint32Value();
    Napi::Function cb = info[2].As<Napi::Function>();
    auto worker = new SimpleAsyncWork<MaaStatus>(
        cb,
        [handle, id]() { return MaaResourceWait(handle, id); },
        [](auto env, auto res) { return Napi::Number::New(env, res); });
    worker->Queue();
    return info.Env().Undefined();
}

Napi::Value resource_loaded(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaResourceAPI>>().Data();
    return Napi::Boolean::New(info.Env(), MaaResourceLoaded(handle));
}

Napi::Value resource_get_hash(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaResourceAPI>>().Data();
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
    auto handle = info[0].As<Napi::External<MaaResourceAPI>>().Data();
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
    exports["resource_create"] = Napi::Function::New(env, resource_create, "MaaResourceCreate");
    exports["resource_post_path"] =
        Napi::Function::New(env, resource_post_path, "MaaResourcePostPath");
    exports["resource_clear"] = Napi::Function::New(env, resource_clear, "MaaResourceClear");
    exports["resource_status"] = Napi::Function::New(env, resource_status, "MaaResourceStatus");
    exports["resource_wait"] = Napi::Function::New(env, resource_wait, "MaaResourceWait");
    exports["resource_loaded"] = Napi::Function::New(env, resource_loaded, "MaaResourceLoaded");
    exports["resource_get_hash"] =
        Napi::Function::New(env, resource_get_hash, "MaaResourceGetHash");
    exports["resource_get_task_list"] =
        Napi::Function::New(env, resource_get_task_list, "MaaResourceGetTaskList");
}
