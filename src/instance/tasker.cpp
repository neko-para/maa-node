#include "../include/info.h"
#include "../include/loader.h"
#include "../include/wrapper.h"

#include <MaaFramework/MaaAPI.h>

std::optional<Napi::External<TaskerInfo>>
    tasker_create(Napi::Env env, std::optional<Napi::Function> callback)
{
    MaaNotificationCallback cb = nullptr;
    CallbackContext* ctx = nullptr;
    MaaTasker* handle = nullptr;

    if (callback) {
        cb = NotificationCallback;
        ctx = new CallbackContext { env, callback.value(), "NotificationCallback" };
    }

    handle = MaaTaskerCreate(cb, ctx);

    if (handle) {
        return Napi::External<TaskerInfo>::New(
            env,
            new TaskerInfo { handle, ctx },
            &DeleteFinalizer<TaskerInfo*>);
    }
    else {
        delete ctx;
        return std::nullopt;
    }
}

void tasker_destroy(Napi::External<TaskerInfo> info)
{
    info.Data()->dispose();
}

bool tasker_bind_resource(
    Napi::External<TaskerInfo> info,
    std::optional<Napi::External<ResourceInfo>> res_info)
{
    if (res_info) {
        if (MaaTaskerBindResource(info.Data()->handle, res_info->Data()->handle)) {
            info.Data()->resource = Napi::Persistent(res_info.value());
            return true;
        }
        else {
            return false;
        }
    }
    else {
        if (MaaTaskerBindResource(info.Data()->handle, nullptr)) {
            info.Data()->resource.Reset();
            return true;
        }
        else {
            return false;
        }
    }
}

bool tasker_bind_controller(
    Napi::External<TaskerInfo> info,
    std::optional<Napi::External<ControllerInfo>> ctrl_info)
{
    if (ctrl_info) {
        if (MaaTaskerBindController(info.Data()->handle, ctrl_info->Data()->handle)) {
            info.Data()->controller = Napi::Persistent(ctrl_info.value());
            return true;
        }
        else {
            return false;
        }
    }
    else {
        if (MaaTaskerBindResource(info.Data()->handle, nullptr)) {
            info.Data()->controller.Reset();
            return true;
        }
        else {
            return false;
        }
    }
}

bool tasker_inited(Napi::External<TaskerInfo> info)
{
    return MaaTaskerInited(info.Data()->handle);
}

MaaTaskId
    tasker_post_pipeline(Napi::External<TaskerInfo> info, std::string entry, std::string overr)
{
    return MaaTaskerPostPipeline(info.Data()->handle, entry.c_str(), overr.c_str());
}

MaaTaskId
    tasker_post_recognition(Napi::External<TaskerInfo> info, std::string entry, std::string overr)
{
    return MaaTaskerPostRecognition(info.Data()->handle, entry.c_str(), overr.c_str());
}

MaaTaskId tasker_post_action(Napi::External<TaskerInfo> info, std::string entry, std::string overr)
{
    return MaaTaskerPostAction(info.Data()->handle, entry.c_str(), overr.c_str());
}

MaaTaskId tasker_override_pipeline(Napi::External<TaskerInfo> info, MaaTaskId id, std::string overr)
{
    return MaaTaskerOverridePipeline(info.Data()->handle, id, overr.c_str());
}

MaaStatus tasker_status(Napi::External<TaskerInfo> info, MaaTaskId id)
{
    return MaaTaskerStatus(info.Data()->handle, id);
}

Napi::Promise tasker_wait(Napi::External<TaskerInfo> info, MaaTaskId id)
{
    auto handle = info.Data()->handle;
    auto worker = new SimpleAsyncWork<MaaStatus>(
        info.Env(),
        [handle, id]() { return MaaTaskerWait(handle, id); },
        [](auto env, auto res) { return Napi::Number::New(env, res); });
    worker->Queue();
    return worker->Promise();
}

bool tasker_running(Napi::External<TaskerInfo> info)
{
    return MaaTaskerRunning(info.Data()->handle);
}

bool tasker_post_stop(Napi::External<TaskerInfo> info)
{
    return MaaTaskerPostStop(info.Data()->handle);
}

std::optional<Napi::External<ResourceInfo>> tasker_get_resource(Napi::External<TaskerInfo> info)
{
    if (info.Data()->resource.IsEmpty()) {
        return std::nullopt;
    }
    else {
        return info.Data()->resource.Value();
    }
}

std::optional<Napi::External<ControllerInfo>> tasker_get_controller(Napi::External<TaskerInfo> info)
{
    if (info.Data()->controller.IsEmpty()) {
        return std::nullopt;
    }
    else {
        return info.Data()->controller.Value();
    }
}

bool tasker_clear_cache(Napi::External<TaskerInfo> info)
{
    return MaaTaskerClearCache(info.Data()->handle);
}

/*
Napi::Value register_custom_recognizer(const Napi::CallbackInfo& info)
{
    CheckCount(info, 3);
    auto handle_info = TaskerInfo::FromValue(info[0]);
    auto name = CheckAsString(info[1]);
    auto func = CheckAsFunction(info[2]);
    auto ctx = new CallbackContext(info.Env(), func, "CustomActionRun");
    auto ret =
        MaaRegisterCustomRecognizer(handle_info->handle, name.c_str(), &custom_recognizer_api,
ctx); if (ret) { auto old_ctx = handle_info->custom_recognizers[name];
        handle_info->custom_actions[name] = ctx;
        if (old_ctx) {
            delete old_ctx;
        }
    }
    return Napi::Boolean::New(info.Env(), ret);
}

Napi::Value unregister_custom_recognizer(const Napi::CallbackInfo& info)
{
    CheckCount(info, 2);
    auto handle_info = TaskerInfo::FromValue(info[0]);
    auto name = CheckAsString(info[1]);

    auto ret = MaaUnregisterCustomRecognizer(handle_info->handle, name.c_str());
    if (ret) {
        delete handle_info->custom_recognizers[name];
        handle_info->custom_recognizers.erase(name);
    }
    return Napi::Boolean::New(info.Env(), ret);
}

Napi::Value clear_custom_recognizer(const Napi::CallbackInfo& info)
{
    CheckCount(info, 1);
    auto handle_info = TaskerInfo::FromValue(info[0]);

    auto ret = MaaClearCustomRecognizer(handle_info->handle);
    if (ret) {
        handle_info->ClearRecos();
    }
    return Napi::Boolean::New(info.Env(), ret);
}

Napi::Value register_custom_action(const Napi::CallbackInfo& info)
{
    CheckCount(info, 3);
    auto handle_info = TaskerInfo::FromValue(info[0]);
    auto name = CheckAsString(info[1]);
    auto func = CheckAsFunction(info[2]);
    auto ctx = new CallbackContext(info.Env(), func, "CustomActionRun");
    auto ret = MaaRegisterCustomAction(handle_info->handle, name.c_str(), &custom_action_api,
ctx); if (ret) { auto old_ctx = handle_info->custom_actions[name];
        handle_info->custom_actions[name] = ctx;
        if (old_ctx) {
            delete old_ctx;
        }
    }
    return Napi::Boolean::New(info.Env(), ret);
}

Napi::Value unregister_custom_action(const Napi::CallbackInfo& info)
{
    CheckCount(info, 2);
    auto handle_info = TaskerInfo::FromValue(info[0]);
    auto name = CheckAsString(info[1]);

    auto ret = MaaUnregisterCustomAction(handle_info->handle, name.c_str());
    if (ret) {
        delete handle_info->custom_actions[name];
        handle_info->custom_actions.erase(name);
    }
    return Napi::Boolean::New(info.Env(), ret);
}

Napi::Value clear_custom_action(const Napi::CallbackInfo& info)
{
    CheckCount(info, 1);
    auto handle_info = TaskerInfo::FromValue(info[0]);

    auto ret = MaaClearCustomAction(handle_info->handle);
    if (ret) {
        handle_info->ClearActs();
    }
    return Napi::Boolean::New(info.Env(), ret);
}
*/

void load_instance_tasker(Napi::Env env, Napi::Object& exports)
{
    BIND(tasker_create);
    BIND(tasker_destroy);
    BIND(tasker_bind_resource);
    BIND(tasker_bind_controller);
    BIND(tasker_inited);
    BIND(tasker_post_pipeline);
    BIND(tasker_post_recognition);
    BIND(tasker_post_action);
    BIND(tasker_override_pipeline);
    BIND(tasker_status);
    BIND(tasker_wait);
    BIND(tasker_running);
    BIND(tasker_post_stop);
    BIND(tasker_get_resource);
    BIND(tasker_get_controller);
    BIND(tasker_clear_cache);
}
