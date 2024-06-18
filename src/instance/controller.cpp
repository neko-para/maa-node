#include "../include/helper.h"
#include "../include/loader.h"

#include <MaaFramework/MaaAPI.h>

#include <iostream>

void ControllerFinalzer(Napi::Env env, MaaControllerHandle handle, void* hint)
{
    std::cerr << "destroy controller" << std::endl;
    MaaControllerDestroy(handle);
    if (hint) {
        auto ctx = reinterpret_cast<CallbackContext*>(hint);
        delete ctx;
    }
}

Napi::Value adb_controller_create(const Napi::CallbackInfo& info)
{
    auto adb_path = info[0].As<Napi::String>().Utf8Value();
    auto adb_serial = info[1].As<Napi::String>().Utf8Value();
    auto type = info[2].As<Napi::Number>().Int32Value();
    auto config = info[3].As<Napi::String>().Utf8Value();
    auto agent_path = info[4].As<Napi::String>().Utf8Value();

    MaaControllerCallback cb = nullptr;
    CallbackContext* ctx = nullptr;
    MaaControllerHandle handle = nullptr;

    if (!info[5].IsNull()) {
        auto callback = info[5].As<Napi::Function>();

        cb = TrivialCallback;
        ctx = new CallbackContext { info.Env(), callback, "TrivialCallback" };
    }

    handle = MaaAdbControllerCreateV2(
        adb_path.c_str(),
        adb_serial.c_str(),
        type,
        config.c_str(),
        agent_path.c_str(),
        cb,
        ctx);

    if (handle) {
        return Napi::External<MaaControllerAPI>::New(info.Env(), handle, ControllerFinalzer, ctx);
    }
    else {
        delete ctx;
        return info.Env().Null();
    }
}

Napi::Value controller_post_connection(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaControllerAPI>>().Data();
    auto ctrlId = MaaControllerPostConnection(handle);
    return Napi::Number::New(info.Env(), ctrlId);
}

Napi::Value controller_post_screencap(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaControllerAPI>>().Data();
    auto ctrlId = MaaControllerPostScreencap(handle);
    return Napi::Number::New(info.Env(), ctrlId);
}

Napi::Value controller_status(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaControllerAPI>>().Data();
    auto id = info[1].As<Napi::Number>().Uint32Value();
    return Napi::Number::New(info.Env(), MaaControllerStatus(handle, id));
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

Napi::Value controller_connected(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaControllerAPI>>().Data();
    return Napi::Boolean::New(info.Env(), MaaControllerConnected(handle));
}

Napi::Value controller_get_image(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaControllerAPI>>().Data();
    auto value = info[1].As<Napi::External<MaaImageBuffer>>().Data();
    return Napi::Boolean::New(info.Env(), MaaControllerGetImage(handle, value));
}

Napi::Value controller_get_uuid(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaControllerAPI>>().Data();
    StringBuffer buffer;
    auto ret = MaaControllerGetUUID(handle, buffer);
    if (ret) {
        return Napi::String::New(info.Env(), buffer);
    }
    else {
        return info.Env().Null();
    }
}

void load_instance_controller(Napi::Env env, Napi::Object& exports)
{
    exports["adb_controller_create"] =
        Napi::Function::New(env, adb_controller_create, "MaaAdbControllerCreateV2");
    exports["controller_post_connection"] =
        Napi::Function::New(env, controller_post_connection, "MaaControllerPostConnection");
    exports["controller_post_screencap"] =
        Napi::Function::New(env, controller_post_screencap, "MaaControllerPostScreencap");
    exports["controller_status"] =
        Napi::Function::New(env, controller_status, "MaaControllerStatus");
    exports["controller_wait"] = Napi::Function::New(env, controller_wait, "MaaControllerWait");
    exports["controller_connected"] =
        Napi::Function::New(env, controller_connected, "MaaControllerConnected");
    exports["controller_get_image"] =
        Napi::Function::New(env, controller_get_image, "MaaControllerGetImage");
    exports["controller_get_uuid"] =
        Napi::Function::New(env, controller_get_image, "MaaControllerGetUUID");
}
