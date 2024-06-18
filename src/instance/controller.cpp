#include "../include/helper.h"
#include "../include/info.h"
#include "../include/loader.h"

#include <MaaFramework/MaaAPI.h>

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
        return Napi::External<ControllerInfo>::New(
            info.Env(),
            new ControllerInfo { handle, ctx },
            &DeleteFinalizer<ControllerInfo*>);
    }
    else {
        delete ctx;
        return info.Env().Null();
    }
}

Napi::Value set_controller_option(const Napi::CallbackInfo& info)
{
    auto handle = ControllerInfo::FromValue(info[0])->handle;
    auto key = info[1].As<Napi::String>().Utf8Value();
    if (key == "ScreenshotTargetLongSide") {
        auto val = info[2].As<Napi::Number>().Int32Value();
        return Napi::Boolean::New(
            info.Env(),
            MaaControllerSetOption(
                handle,
                MaaCtrlOption_ScreenshotTargetLongSide,
                &val,
                sizeof(val)));
    }
    else if (key == "ScreenshotTargetShortSide") {
        auto val = info[2].As<Napi::Number>().Int32Value();
        return Napi::Boolean::New(
            info.Env(),
            MaaControllerSetOption(
                handle,
                MaaCtrlOption_ScreenshotTargetShortSide,
                &val,
                sizeof(val)));
    }
    else if (key == "DefaultAppPackageEntry") {
        auto val = info[2].As<Napi::String>().Utf8Value();
        return Napi::Boolean::New(
            info.Env(),
            MaaControllerSetOption(
                handle,
                MaaCtrlOption_DefaultAppPackageEntry,
                val.data(),
                val.length()));
    }
    else if (key == "DefaultAppPackage") {
        auto val = info[2].As<Napi::String>().Utf8Value();
        return Napi::Boolean::New(
            info.Env(),
            MaaControllerSetOption(
                handle,
                MaaCtrlOption_DefaultAppPackage,
                val.data(),
                val.length()));
    }
    else if (key == "Recording") {
        auto val = info[2].As<Napi::Boolean>().Value();
        return Napi::Boolean::New(
            info.Env(),
            MaaControllerSetOption(handle, MaaCtrlOption_Recording, &val, sizeof(val)));
    }
    else {
        return Napi::Boolean::New(info.Env(), false);
    }
}

Napi::Value controller_post_connection(const Napi::CallbackInfo& info)
{
    auto handle = ControllerInfo::FromValue(info[0])->handle;
    auto ctrlId = MaaControllerPostConnection(handle);
    return Napi::Number::New(info.Env(), ctrlId);
}

Napi::Value controller_post_screencap(const Napi::CallbackInfo& info)
{
    auto handle = ControllerInfo::FromValue(info[0])->handle;
    auto ctrlId = MaaControllerPostScreencap(handle);
    return Napi::Number::New(info.Env(), ctrlId);
}

Napi::Value controller_status(const Napi::CallbackInfo& info)
{
    auto handle = ControllerInfo::FromValue(info[0])->handle;
    auto id = info[1].As<Napi::Number>().Uint32Value();
    return Napi::Number::New(info.Env(), MaaControllerStatus(handle, id));
}

Napi::Value controller_wait(const Napi::CallbackInfo& info)
{
    auto handle = ControllerInfo::FromValue(info[0])->handle;
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
    auto handle = ControllerInfo::FromValue(info[0])->handle;
    return Napi::Boolean::New(info.Env(), MaaControllerConnected(handle));
}

Napi::Value controller_get_image(const Napi::CallbackInfo& info)
{
    auto handle = ControllerInfo::FromValue(info[0])->handle;
    auto value = info[1].As<Napi::External<MaaImageBuffer>>().Data();
    return Napi::Boolean::New(info.Env(), MaaControllerGetImage(handle, value));
}

Napi::Value controller_get_uuid(const Napi::CallbackInfo& info)
{
    auto handle = ControllerInfo::FromValue(info[0])->handle;
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
    BIND(adb_controller_create);
    BIND(set_controller_option);
    BIND(controller_post_connection);
    BIND(controller_post_screencap);
    BIND(controller_status);
    BIND(controller_wait);
    BIND(controller_connected);
    BIND(controller_get_image);
    BIND(controller_get_uuid);
}
