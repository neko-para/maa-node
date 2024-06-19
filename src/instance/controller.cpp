#include "../include/helper.h"
#include "../include/info.h"
#include "../include/loader.h"

#include <MaaFramework/MaaAPI.h>

Napi::Value adb_controller_create(const Napi::CallbackInfo& info)
{
    auto adb_path = info[0].As<Napi::String>().Utf8Value();
    auto address = info[1].As<Napi::String>().Utf8Value();
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
        address.c_str(),
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

Napi::Value win32_controller_create(const Napi::CallbackInfo& info)
{
    auto hwnd = info[0].As<Napi::External<void>>().Data();
    auto type = info[1].As<Napi::Number>().Int32Value();

    MaaControllerCallback cb = nullptr;
    CallbackContext* ctx = nullptr;
    MaaControllerHandle handle = nullptr;

    if (!info[2].IsNull()) {
        auto callback = info[2].As<Napi::Function>();

        cb = TrivialCallback;
        ctx = new CallbackContext { info.Env(), callback, "TrivialCallback" };
    }

    handle = MaaWin32ControllerCreate(hwnd, type, cb, ctx);

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

Napi::Value controller_post_click(const Napi::CallbackInfo& info)
{
    auto handle = ControllerInfo::FromValue(info[0])->handle;
    auto x = info[1].As<Napi::Number>().Int32Value();
    auto y = info[2].As<Napi::Number>().Int32Value();
    auto ctrlId = MaaControllerPostClick(handle, x, y);
    return Napi::Number::New(info.Env(), ctrlId);
}

Napi::Value controller_post_swipe(const Napi::CallbackInfo& info)
{
    auto handle = ControllerInfo::FromValue(info[0])->handle;
    auto x1 = info[1].As<Napi::Number>().Int32Value();
    auto y1 = info[2].As<Napi::Number>().Int32Value();
    auto x2 = info[3].As<Napi::Number>().Int32Value();
    auto y2 = info[4].As<Napi::Number>().Int32Value();
    auto duration = info[5].As<Napi::Number>().Int32Value();
    auto ctrlId = MaaControllerPostSwipe(handle, x1, y1, x2, y2, duration);
    return Napi::Number::New(info.Env(), ctrlId);
}

Napi::Value controller_post_press_key(const Napi::CallbackInfo& info)
{
    auto handle = ControllerInfo::FromValue(info[0])->handle;
    auto key = info[1].As<Napi::Number>().Int32Value();
    auto ctrlId = MaaControllerPostPressKey(handle, key);
    return Napi::Number::New(info.Env(), ctrlId);
}

Napi::Value controller_post_input_text(const Napi::CallbackInfo& info)
{
    auto handle = ControllerInfo::FromValue(info[0])->handle;
    auto text = info[1].As<Napi::String>().Utf8Value();
    auto ctrlId = MaaControllerPostInputText(handle, text.c_str());
    return Napi::Number::New(info.Env(), ctrlId);
}

Napi::Value controller_post_start_app(const Napi::CallbackInfo& info)
{
    auto handle = ControllerInfo::FromValue(info[0])->handle;
    auto intent = info[1].As<Napi::String>().Utf8Value();
    auto ctrlId = MaaControllerPostStartApp(handle, intent.c_str());
    return Napi::Number::New(info.Env(), ctrlId);
}

Napi::Value controller_post_stop_app(const Napi::CallbackInfo& info)
{
    auto handle = ControllerInfo::FromValue(info[0])->handle;
    auto intent = info[1].As<Napi::String>().Utf8Value();
    auto ctrlId = MaaControllerPostStopApp(handle, intent.c_str());
    return Napi::Number::New(info.Env(), ctrlId);
}

Napi::Value controller_post_touch_down(const Napi::CallbackInfo& info)
{
    auto handle = ControllerInfo::FromValue(info[0])->handle;
    auto contact = info[1].As<Napi::Number>().Int32Value();
    auto x = info[2].As<Napi::Number>().Int32Value();
    auto y = info[3].As<Napi::Number>().Int32Value();
    auto pressure = info[4].As<Napi::Number>().Int32Value();
    auto ctrlId = MaaControllerPostTouchDown(handle, contact, x, y, pressure);
    return Napi::Number::New(info.Env(), ctrlId);
}

Napi::Value controller_post_touch_move(const Napi::CallbackInfo& info)
{
    auto handle = ControllerInfo::FromValue(info[0])->handle;
    auto contact = info[1].As<Napi::Number>().Int32Value();
    auto x = info[2].As<Napi::Number>().Int32Value();
    auto y = info[3].As<Napi::Number>().Int32Value();
    auto pressure = info[4].As<Napi::Number>().Int32Value();
    auto ctrlId = MaaControllerPostTouchMove(handle, contact, x, y, pressure);
    return Napi::Number::New(info.Env(), ctrlId);
}

Napi::Value controller_post_touch_up(const Napi::CallbackInfo& info)
{
    auto handle = ControllerInfo::FromValue(info[0])->handle;
    auto contact = info[1].As<Napi::Number>().Int32Value();
    auto ctrlId = MaaControllerPostTouchUp(handle, contact);
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
    auto worker = new SimpleAsyncWork<MaaStatus>(
        info.Env(),
        [handle, id]() { return MaaControllerWait(handle, id); },
        [](auto env, auto res) { return Napi::Number::New(env, res); });
    worker->Queue();
    return worker->Promise();
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
    BIND(win32_controller_create);
    BIND(set_controller_option);
    BIND(controller_post_connection);
    BIND(controller_post_click);
    BIND(controller_post_swipe);
    BIND(controller_post_press_key);
    BIND(controller_post_input_text);
    BIND(controller_post_start_app);
    BIND(controller_post_stop_app);
    BIND(controller_post_touch_down);
    BIND(controller_post_touch_move);
    BIND(controller_post_touch_up);
    BIND(controller_post_screencap);
    BIND(controller_status);
    BIND(controller_wait);
    BIND(controller_connected);
    BIND(controller_get_image);
    BIND(controller_get_uuid);
}
