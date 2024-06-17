#include "../include/helper.h"
#include "../include/loader.h"

#include <MaaFramework/MaaAPI.h>

#include <future>
#include <iostream>

struct CallbackContext
{
    Napi::Function fn;
    Napi::ThreadSafeFunction tsfn;
};

void ControllerFinalzer(Napi::Env env, MaaControllerHandle handle, void* hint)
{
    std::cerr << "destroy controller" << std::endl;
    MaaControllerDestroy(handle);
    auto ctx = reinterpret_cast<CallbackContext*>(hint);
    ctx->tsfn.Release();
    delete ctx;
}

Napi::Value adb_controller_create(const Napi::CallbackInfo& info)
{
    auto adb_path = info[0].As<Napi::String>().Utf8Value();
    auto adb_serial = info[1].As<Napi::String>().Utf8Value();
    auto type = info[2].As<Napi::Number>().Int32Value();
    auto config = info[3].As<Napi::String>().Utf8Value();
    auto agent_path = info[4].As<Napi::String>().Utf8Value();
    auto callback = info[5].As<Napi::Function>();
    auto ctx = new CallbackContext {
        callback,
        Napi::ThreadSafeFunction::New(info.Env(), callback, "TrivialCallback", 0, 1)
    };
    auto handle = MaaAdbControllerCreateV2(
        adb_path.c_str(),
        adb_serial.c_str(),
        type,
        config.c_str(),
        agent_path.c_str(),
        [](MaaStringView msg, MaaStringView details, MaaCallbackTransparentArg arg) {
            auto ctx = reinterpret_cast<CallbackContext*>(arg);
            std::string msg_str = msg;
            std::string details_str = details;
            std::promise<void> promise;
            std::future<void> future = promise.get_future();
            ctx->tsfn.BlockingCall(
                [msg_str, details_str, &promise](Napi::Env env, Napi::Function fn) {
                    Napi::Value result = fn.Call(
                        { Napi::String::New(env, msg_str), Napi::String::New(env, details_str) });
                    if (result.IsPromise()) {
                        Napi::Object resultObject = result.As<Napi::Object>();
                        resultObject.Get("then").As<Napi::Function>().Call(
                            resultObject,
                            {

                                Napi::Function::New(
                                    env,
                                    [&promise](const Napi::CallbackInfo& info) {
                                        promise.set_value();
                                        return info.Env().Undefined();
                                    }),
                                Napi::Function::New(
                                    env,
                                    [&promise](const Napi::CallbackInfo& info) {
                                        promise.set_value();
                                        info.Env().Undefined();
                                    }),
                            });
                    }
                    else {
                        promise.set_value();
                    }
                });
            future.get();
        },
        ctx);
    if (handle) {
        return Napi::External<MaaControllerAPI>::New(info.Env(), handle, ControllerFinalzer, ctx);
    }
    else {
        return info.Env().Null();
    }
}

Napi::Value controller_post_connection(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaControllerAPI>>().Data();
    auto ctrlId = MaaControllerPostConnection(handle);
    return Napi::Number::New(info.Env(), ctrlId);
}

Napi::Value controller_wait(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaControllerAPI>>().Data();
    auto id = info[1].As<Napi::Number>().Uint32Value();
    Napi::Function cb = info[2].As<Napi::Function>();
    auto worker = new SimpleAsyncWork<MaaStatus>(
        cb,
        [handle, id]() { return MaaControllerWait(handle, id); },
        [](auto env, auto res) { return Napi::Number::New(env, res); });
    worker->Queue();
    return info.Env().Undefined();
}

void load_instance_controller(Napi::Env env, Napi::Object& exports)
{
    exports["adb_controller_create"] =
        Napi::Function::New(env, adb_controller_create, "MaaAdbControllerCreateV2");
    exports["controller_post_connection"] =
        Napi::Function::New(env, controller_post_connection, "MaaControllerPostConnection");
    exports["controller_wait"] = Napi::Function::New(env, controller_wait, "MaaControllerWait");
}
