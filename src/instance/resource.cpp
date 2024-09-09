#include "../include/info.h"
#include "../include/loader.h"
#include "../include/utils.h"
#include "../include/wrapper.h"

#include <MaaFramework/MaaAPI.h>

std::optional<Napi::External<ResourceInfo>>
    resource_create(Napi::Env env, std::optional<Napi::Function> callback)
{
    MaaNotificationCallback cb = nullptr;
    CallbackContext* ctx = nullptr;
    MaaResource* handle = nullptr;

    if (callback) {
        cb = NotificationCallback;
        ctx = new CallbackContext { env, callback.value(), "NotificationCallback" };
    }

    handle = MaaResourceCreate(cb, ctx);

    if (handle) {
        return Napi::External<ResourceInfo>::New(
            env,
            new ResourceInfo { handle, ctx },
            &DeleteFinalizer<ResourceInfo*>);
    }
    else {
        delete ctx;
        return std::nullopt;
    }
}

void resource_destroy(Napi::External<ResourceInfo> info)
{
    info.Data()->dispose();
}

MaaResId resource_post_path(Napi::External<ResourceInfo> info, std::string path)
{
    return MaaResourcePostPath(info.Data()->handle, path.c_str());
}

bool resource_clear(Napi::External<ResourceInfo> info)
{
    return MaaResourceClear(info.Data()->handle);
}

MaaStatus resource_status(Napi::External<ResourceInfo> info, MaaResId id)
{
    return MaaResourceStatus(info.Data()->handle, id);
}

Napi::Promise resource_wait(Napi::Env env, Napi::External<ResourceInfo> info, MaaResId id)
{
    auto handle = info.Data()->handle;
    auto worker = new SimpleAsyncWork<MaaStatus, "resource_wait">(env, [handle, id]() {
        return MaaResourceWait(handle, id);
    });
    worker->Queue();
    return worker->Promise();
}

bool resource_loaded(Napi::External<ResourceInfo> info)
{
    return MaaResourceLoaded(info.Data()->handle);
}

std::optional<std::string> resource_get_hash(Napi::External<ResourceInfo> info)
{
    StringBuffer buffer;
    auto ret = MaaResourceGetHash(info.Data()->handle, buffer);
    if (ret) {
        return Napi::String::New(info.Env(), buffer.str());
    }
    else {
        return std::nullopt;
    }
}

std::optional<std::vector<std::string>> resource_get_task_list(Napi::External<ResourceInfo> info)
{
    StringListBuffer buffer;
    auto ret = MaaResourceGetTaskList(info.Data()->handle, buffer);
    if (ret) {
        return buffer.as_vector([](StringBufferRefer buf) { return buf.str(); });
    }
    else {
        return std::nullopt;
    }
}

void load_instance_resource(
    Napi::Env env,
    Napi::Object& exports,
    Napi::External<ExtContextInfo> context)
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
