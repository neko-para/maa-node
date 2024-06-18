#include "../include/helper.h"
#include "../include/loader.h"

#include <MaaFramework/MaaAPI.h>

#include <iostream>

void InstanceFinalzer(Napi::Env env, MaaInstanceHandle handle, void* hint)
{
    std::cerr << "destroy instance" << std::endl;
    MaaDestroy(handle);
    if (hint) {
        auto ctx = reinterpret_cast<CallbackContext*>(hint);
        delete ctx;
    }
}

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
        return Napi::External<MaaInstanceAPI>::New(info.Env(), handle, InstanceFinalzer, ctx);
    }
    else {
        delete ctx;
        return info.Env().Null();
    }
}

Napi::Value bind_resource(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaInstanceAPI>>().Data();
    auto value = ExternalOrNull<MaaResourceAPI>(info[1]);
    return Napi::Boolean::New(info.Env(), MaaBindResource(handle, value.value_or(nullptr)));
}

Napi::Value bind_controller(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaInstanceAPI>>().Data();
    auto value = ExternalOrNull<MaaControllerAPI>(info[1]);
    return Napi::Boolean::New(info.Env(), MaaBindController(handle, value.value_or(nullptr)));
}

Napi::Value inited(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaInstanceAPI>>().Data();
    return Napi::Boolean::New(info.Env(), MaaInited(handle));
}

Napi::Value post_task(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaInstanceAPI>>().Data();
    auto entry = info[1].As<Napi::String>().Utf8Value();
    auto param = info[2].As<Napi::String>().Utf8Value();
    auto taskid = MaaPostTask(handle, entry.c_str(), param.c_str());
    return Napi::Number::New(info.Env(), taskid);
}

Napi::Value post_recognition(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaInstanceAPI>>().Data();
    auto entry = info[1].As<Napi::String>().Utf8Value();
    auto param = info[2].As<Napi::String>().Utf8Value();
    auto taskid = MaaPostRecognition(handle, entry.c_str(), param.c_str());
    return Napi::Number::New(info.Env(), taskid);
}

Napi::Value post_action(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaInstanceAPI>>().Data();
    auto entry = info[1].As<Napi::String>().Utf8Value();
    auto param = info[2].As<Napi::String>().Utf8Value();
    auto taskid = MaaPostAction(handle, entry.c_str(), param.c_str());
    return Napi::Number::New(info.Env(), taskid);
}

Napi::Value set_task_param(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaInstanceAPI>>().Data();
    auto id = info[1].As<Napi::Number>().Uint32Value();
    auto param = info[2].As<Napi::String>().Utf8Value();
    return Napi::Boolean::New(info.Env(), MaaSetTaskParam(handle, id, param.c_str()));
}

Napi::Value task_status(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaInstanceAPI>>().Data();
    auto id = info[1].As<Napi::Number>().Uint32Value();
    return Napi::Number::New(info.Env(), MaaTaskStatus(handle, id));
}

Napi::Value wait_task(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaInstanceAPI>>().Data();
    auto id = info[1].As<Napi::Number>().Uint32Value();
    Napi::Function cb = info[2].As<Napi::Function>();
    auto worker = new SimpleAsyncWork<MaaStatus>(
        cb,
        [handle, id]() { return MaaWaitTask(handle, id); },
        [](auto env, auto res) { return Napi::Number::New(env, res); });
    worker->Queue();
    return info.Env().Undefined();
}

Napi::Value running(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaInstanceAPI>>().Data();
    return Napi::Boolean::New(info.Env(), MaaRunning(handle));
}

Napi::Value post_stop(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaInstanceAPI>>().Data();
    return Napi::Boolean::New(info.Env(), MaaPostStop(handle));
}

void load_instance_instance(Napi::Env env, Napi::Object& exports)
{
    BIND(create);
    BIND(bind_resource);
    BIND(bind_controller);
    BIND(inited);
    BIND(post_task);
    BIND(post_recognition);
    BIND(post_action);
    BIND(set_task_param);
    BIND(task_status);
    BIND(wait_task);
    BIND(running);
    BIND(post_stop);
}
