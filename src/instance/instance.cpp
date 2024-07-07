#include "../include/helper.h"
#include "../include/info.h"
#include "../include/loader.h"

#include <MaaFramework/MaaAPI.h>

Napi::Value create(const Napi::CallbackInfo& info)
{
    CheckCount(info, 1);
    MaaInstanceCallback cb = nullptr;
    CallbackContext* ctx = nullptr;
    MaaInstanceHandle handle = nullptr;

    if (!info[5].IsNull()) {
        auto callback = CheckAsFunction(info[0]);

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

Napi::Value destroy(const Napi::CallbackInfo& info)
{
    CheckCount(info, 1);
    auto handle = InstanceInfo::FromValue(info[0]);
    handle->dispose();
    return info.Env().Undefined();
}

Napi::Value bind_resource(const Napi::CallbackInfo& info)
{
    CheckCount(info, 2);
    auto handle_info = InstanceInfo::FromValue(info[0]);
    auto value_info = ResourceInfo::FromValueOrNull(info[1]);
    auto value = value_info ? value_info->handle : nullptr;
    auto ret = MaaBindResource(handle_info->handle, value);
    if (ret) {
        if (value_info) {
            handle_info->resource = Napi::Persistent(CheckAsExternal<ResourceInfo>(info[1]));
        }
        else {
            handle_info->resource.Reset();
        }
    }
    return Napi::Boolean::New(info.Env(), ret);
}

Napi::Value bind_controller(const Napi::CallbackInfo& info)
{
    CheckCount(info, 2);
    auto handle_info = InstanceInfo::FromValue(info[0]);
    auto value_info = ControllerInfo::FromValueOrNull(info[1]);
    auto value = value_info ? value_info->handle : nullptr;
    auto ret = MaaBindController(handle_info->handle, value);
    if (ret) {
        if (value_info) {
            handle_info->controller = Napi::Persistent(CheckAsExternal<ControllerInfo>(info[1]));
        }
        else {
            handle_info->controller.Reset();
        }
    }
    return Napi::Boolean::New(info.Env(), ret);
}

Napi::Value inited(const Napi::CallbackInfo& info)
{
    CheckCount(info, 1);
    auto handle = InstanceInfo::HandleFromValue(info[0]);
    return Napi::Boolean::New(info.Env(), MaaInited(handle));
}

Napi::Value register_custom_recognizer(const Napi::CallbackInfo& info)
{
    CheckCount(info, 3);
    auto handle_info = InstanceInfo::FromValue(info[0]);
    auto name = CheckAsString(info[1]);
    auto func = CheckAsFunction(info[2]);
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
    CheckCount(info, 2);
    auto handle_info = InstanceInfo::FromValue(info[0]);
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
    auto handle_info = InstanceInfo::FromValue(info[0]);

    auto ret = MaaClearCustomRecognizer(handle_info->handle);
    if (ret) {
        handle_info->ClearRecos();
    }
    return Napi::Boolean::New(info.Env(), ret);
}

Napi::Value register_custom_action(const Napi::CallbackInfo& info)
{
    CheckCount(info, 3);
    auto handle_info = InstanceInfo::FromValue(info[0]);
    auto name = CheckAsString(info[1]);
    auto func = CheckAsFunction(info[2]);
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
    CheckCount(info, 2);
    auto handle_info = InstanceInfo::FromValue(info[0]);
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
    auto handle_info = InstanceInfo::FromValue(info[0]);

    auto ret = MaaClearCustomAction(handle_info->handle);
    if (ret) {
        handle_info->ClearActs();
    }
    return Napi::Boolean::New(info.Env(), ret);
}

Napi::Value post_task(const Napi::CallbackInfo& info)
{
    CheckCount(info, 3);
    auto handle = InstanceInfo::HandleFromValue(info[0]);
    auto entry = CheckAsString(info[1]);
    auto param = CheckAsString(info[2]);
    auto taskid = MaaPostTask(handle, entry.c_str(), param.c_str());
    return Napi::Number::New(info.Env(), taskid);
}

Napi::Value post_recognition(const Napi::CallbackInfo& info)
{
    CheckCount(info, 3);
    auto handle = InstanceInfo::HandleFromValue(info[0]);
    auto entry = CheckAsString(info[1]);
    auto param = CheckAsString(info[2]);
    auto taskid = MaaPostRecognition(handle, entry.c_str(), param.c_str());
    return Napi::Number::New(info.Env(), taskid);
}

Napi::Value post_action(const Napi::CallbackInfo& info)
{
    CheckCount(info, 3);
    auto handle = InstanceInfo::HandleFromValue(info[0]);
    auto entry = CheckAsString(info[1]);
    auto param = CheckAsString(info[2]);
    auto taskid = MaaPostAction(handle, entry.c_str(), param.c_str());
    return Napi::Number::New(info.Env(), taskid);
}

Napi::Value set_task_param(const Napi::CallbackInfo& info)
{
    CheckCount(info, 3);
    auto handle = InstanceInfo::HandleFromValue(info[0]);
    auto id = CheckAsNumber(info[1]).Uint32Value();
    auto param = CheckAsString(info[2]);
    return Napi::Boolean::New(info.Env(), MaaSetTaskParam(handle, id, param.c_str()));
}

Napi::Value task_status(const Napi::CallbackInfo& info)
{
    CheckCount(info, 2);
    auto handle = InstanceInfo::HandleFromValue(info[0]);
    auto id = CheckAsNumber(info[1]).Uint32Value();
    return Napi::Number::New(info.Env(), MaaTaskStatus(handle, id));
}

Napi::Value wait_task(const Napi::CallbackInfo& info)
{
    CheckCount(info, 2);
    auto handle = InstanceInfo::HandleFromValue(info[0]);
    auto id = CheckAsNumber(info[1]).Uint32Value();
    auto worker = new SimpleAsyncWork<MaaStatus>(
        info.Env(),
        [handle, id]() { return MaaWaitTask(handle, id); },
        [](auto env, auto res) { return Napi::Number::New(env, res); });
    worker->Queue();
    return worker->Promise();
}

Napi::Value running(const Napi::CallbackInfo& info)
{
    CheckCount(info, 1);
    auto handle = InstanceInfo::HandleFromValue(info[0]);
    return Napi::Boolean::New(info.Env(), MaaRunning(handle));
}

Napi::Value post_stop(const Napi::CallbackInfo& info)
{
    CheckCount(info, 1);
    auto handle = InstanceInfo::HandleFromValue(info[0]);
    return Napi::Boolean::New(info.Env(), MaaPostStop(handle));
}

void load_instance_instance(Napi::Env env, Napi::Object& exports)
{
    BIND(create);
    BIND(destroy);
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
