#include "../include/helper.h"
#include "../include/info.h"
#include "../include/loader.h"

#include <MaaFramework/MaaAPI.h>

Napi::Value create(const Napi::CallbackInfo& info)
{
    MaaInstanceCallback cb = nullptr;
    CallbackContext* ctx = nullptr;
    MaaInstanceHandle handle = nullptr;

    if (!info[5].IsNull()) {
        auto callback = info[0].As<Napi::Function>();

        cb = TrivialCallback;
        ctx = new CallbackContext { info.Env(), callback, "TrivialCallback" };
    }

    handle = MaaCreate(cb, ctx);

    if (handle) {
        return Napi::External<InstanceInfo>::New(
            info.Env(),
            new InstanceInfo { handle, ctx },
            &DeleteFinalizer<InstanceInfo*>);
    }
    else {
        delete ctx;
        return info.Env().Null();
    }
}

Napi::Value bind_resource(const Napi::CallbackInfo& info)
{
    auto handle_info = InstanceInfo::FromValue(info[0]);
    auto value_info = ResourceInfo::FromValueOrNull(info[1]);
    auto value = value_info ? value_info->handle : nullptr;
    auto ret = MaaBindResource(handle_info->handle, value);
    if (ret) {
        if (value_info) {
            handle_info->resource = Napi::Persistent(info[1].As<Napi::External<ResourceInfo>>());
        }
        else {
            handle_info->resource.Reset();
        }
    }
    return Napi::Boolean::New(info.Env(), ret);
}

Napi::Value bind_controller(const Napi::CallbackInfo& info)
{
    auto handle_info = InstanceInfo::FromValue(info[0]);
    auto value_info = ControllerInfo::FromValueOrNull(info[1]);
    auto value = value_info ? value_info->handle : nullptr;
    auto ret = MaaBindController(handle_info->handle, value);
    if (ret) {
        if (value_info) {
            handle_info->controller =
                Napi::Persistent(info[1].As<Napi::External<ControllerInfo>>());
        }
        else {
            handle_info->controller.Reset();
        }
    }
    return Napi::Boolean::New(info.Env(), ret);
}

Napi::Value inited(const Napi::CallbackInfo& info)
{
    auto handle = InstanceInfo::HandleFromValue(info[0]);
    return Napi::Boolean::New(info.Env(), MaaInited(handle));
}

Napi::Value register_custom_recognizer(const Napi::CallbackInfo& info)
{
    auto handle_info = InstanceInfo::FromValue(info[0]);
    auto name = info[1].As<Napi::String>().Utf8Value();
    auto func = info[2].As<Napi::Function>();
    auto ctx = new CallbackContext(info.Env(), func, "CustomActionRun");
    auto old_ctx = handle_info->custom_recognizers[name];
    handle_info->custom_actions[name] = ctx;

    auto ret =
        MaaRegisterCustomRecognizer(handle_info->handle, name.c_str(), &custom_recognizer_api, ctx);
    if (ret) {
        if (old_ctx) {
            delete old_ctx;
        }
    }
    return Napi::Boolean::New(info.Env(), ret);
}

Napi::Value unregister_custom_recognizer(const Napi::CallbackInfo& info)
{
    auto handle_info = InstanceInfo::FromValue(info[0]);
    auto name = info[1].As<Napi::String>().Utf8Value();

    auto ret = MaaUnregisterCustomRecognizer(handle_info->handle, name.c_str());
    if (ret) {
        delete handle_info->custom_recognizers[name];
        handle_info->custom_recognizers.erase(name);
    }
    return Napi::Boolean::New(info.Env(), ret);
}

Napi::Value clear_custom_recognizer(const Napi::CallbackInfo& info)
{
    auto handle_info = InstanceInfo::FromValue(info[0]);

    auto ret = MaaClearCustomRecognizer(handle_info->handle);
    if (ret) {
        handle_info->ClearRecos();
    }
    return Napi::Boolean::New(info.Env(), ret);
}

Napi::Value register_custom_action(const Napi::CallbackInfo& info)
{
    auto handle_info = InstanceInfo::FromValue(info[0]);
    auto name = info[1].As<Napi::String>().Utf8Value();
    auto func = info[2].As<Napi::Function>();
    auto ctx = new CallbackContext(info.Env(), func, "CustomActionRun");
    auto old_ctx = handle_info->custom_actions[name];
    handle_info->custom_actions[name] = ctx;

    auto ret = MaaRegisterCustomAction(handle_info->handle, name.c_str(), &custom_action_api, ctx);
    if (ret) {
        if (old_ctx) {
            delete old_ctx;
        }
    }
    return Napi::Boolean::New(info.Env(), ret);
}

Napi::Value unregister_custom_action(const Napi::CallbackInfo& info)
{
    auto handle_info = InstanceInfo::FromValue(info[0]);
    auto name = info[1].As<Napi::String>().Utf8Value();

    auto ret = MaaUnregisterCustomAction(handle_info->handle, name.c_str());
    if (ret) {
        delete handle_info->custom_actions[name];
        handle_info->custom_actions.erase(name);
    }
    return Napi::Boolean::New(info.Env(), ret);
}

Napi::Value clear_custom_action(const Napi::CallbackInfo& info)
{
    auto handle_info = InstanceInfo::FromValue(info[0]);

    auto ret = MaaClearCustomAction(handle_info->handle);
    if (ret) {
        handle_info->ClearActs();
    }
    return Napi::Boolean::New(info.Env(), ret);
}

Napi::Value post_task(const Napi::CallbackInfo& info)
{
    auto handle = InstanceInfo::HandleFromValue(info[0]);
    auto entry = info[1].As<Napi::String>().Utf8Value();
    auto param = info[2].As<Napi::String>().Utf8Value();
    auto taskid = MaaPostTask(handle, entry.c_str(), param.c_str());
    return Napi::Number::New(info.Env(), taskid);
}

Napi::Value post_recognition(const Napi::CallbackInfo& info)
{
    auto handle = InstanceInfo::HandleFromValue(info[0]);
    auto entry = info[1].As<Napi::String>().Utf8Value();
    auto param = info[2].As<Napi::String>().Utf8Value();
    auto taskid = MaaPostRecognition(handle, entry.c_str(), param.c_str());
    return Napi::Number::New(info.Env(), taskid);
}

Napi::Value post_action(const Napi::CallbackInfo& info)
{
    auto handle = InstanceInfo::HandleFromValue(info[0]);
    auto entry = info[1].As<Napi::String>().Utf8Value();
    auto param = info[2].As<Napi::String>().Utf8Value();
    auto taskid = MaaPostAction(handle, entry.c_str(), param.c_str());
    return Napi::Number::New(info.Env(), taskid);
}

Napi::Value set_task_param(const Napi::CallbackInfo& info)
{
    auto handle = InstanceInfo::HandleFromValue(info[0]);
    auto id = info[1].As<Napi::Number>().Uint32Value();
    auto param = info[2].As<Napi::String>().Utf8Value();
    return Napi::Boolean::New(info.Env(), MaaSetTaskParam(handle, id, param.c_str()));
}

Napi::Value task_status(const Napi::CallbackInfo& info)
{
    auto handle = InstanceInfo::HandleFromValue(info[0]);
    auto id = info[1].As<Napi::Number>().Uint32Value();
    return Napi::Number::New(info.Env(), MaaTaskStatus(handle, id));
}

Napi::Value wait_task(const Napi::CallbackInfo& info)
{
    auto handle = InstanceInfo::HandleFromValue(info[0]);
    auto id = info[1].As<Napi::Number>().Uint32Value();
    auto worker = new SimpleAsyncWork<MaaStatus>(
        info.Env(),
        [handle, id]() { return MaaWaitTask(handle, id); },
        [](auto env, auto res) { return Napi::Number::New(env, res); });
    worker->Queue();
    return worker->Promise();
}

Napi::Value running(const Napi::CallbackInfo& info)
{
    auto handle = InstanceInfo::HandleFromValue(info[0]);
    return Napi::Boolean::New(info.Env(), MaaRunning(handle));
}

Napi::Value post_stop(const Napi::CallbackInfo& info)
{
    auto handle = InstanceInfo::HandleFromValue(info[0]);
    return Napi::Boolean::New(info.Env(), MaaPostStop(handle));
}

void load_instance_instance(Napi::Env env, Napi::Object& exports)
{
    BIND(create);
    BIND(bind_resource);
    BIND(bind_controller);
    BIND(inited);
    BIND(register_custom_recognizer);
    BIND(unregister_custom_recognizer);
    BIND(clear_custom_recognizer);
    BIND(register_custom_action);
    BIND(unregister_custom_action);
    BIND(clear_custom_action);
    BIND(post_task);
    BIND(post_recognition);
    BIND(post_action);
    BIND(set_task_param);
    BIND(task_status);
    BIND(wait_task);
    BIND(running);
    BIND(post_stop);
}
