#include "../include/helper.h"
#include "../include/info.h"
#include "../include/loader.h"

#include <MaaFramework/MaaAPI.h>

Napi::Value adb_controller_create(const Napi::CallbackInfo& info)
{
    CheckCount(info, 6);
    auto adb_path = CheckAsString(info[0]);
    auto address = CheckAsString(info[1]);
    auto type = CheckAsNumber(info[2]).Int32Value();
    auto config = CheckAsString(info[3]);
    auto agent_path = CheckAsString(info[4]);

    MaaControllerCallback cb = nullptr;
    CallbackContext* ctx = nullptr;
    MaaControllerHandle handle = nullptr;

    if (!info[5].IsNull()) {
        auto callback = CheckAsFunction(info[5]);

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
    CheckCount(info, 3);
    auto hwnd = CheckAsExternal<void>(info[0]).Data();
    auto type = CheckAsNumber(info[1]).Int32Value();

    MaaControllerCallback cb = nullptr;
    CallbackContext* ctx = nullptr;
    MaaControllerHandle handle = nullptr;

    if (!info[2].IsNull()) {
        auto callback = CheckAsFunction(info[2]);

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

Napi::Value custom_controller_create(const Napi::CallbackInfo& info)
{
    CheckCount(info, 2);

    auto custom_callback = CheckAsFunction(info[0]);
    CallbackContext* custom_ctx =
        new CallbackContext(info.Env(), custom_callback, "CustomControllerCallback");

    MaaControllerCallback cb = nullptr;
    CallbackContext* ctx = nullptr;
    MaaControllerHandle handle = nullptr;

    if (!info[1].IsNull()) {
        auto callback = CheckAsFunction(info[1]);

        cb = TrivialCallback;
        ctx = new CallbackContext { info.Env(), callback, "TrivialCallback" };
    }

    handle = MaaCustomControllerCreate(&custom_controller_api, custom_ctx, cb, ctx);

    if (handle) {
        return Napi::External<ControllerInfo>::New(
            info.Env(),
            new ControllerInfo { handle, ctx, custom_ctx },
            &DeleteFinalizer<ControllerInfo*>);
    }
    else {
        delete custom_ctx;
        delete ctx;
        return info.Env().Null();
    }
}

Napi::Value controller_destroy(const Napi::CallbackInfo& info)
{
    CheckCount(info, 1);
    auto handle = ControllerInfo::FromValue(info[0]);
    handle->dispose();
    return info.Env().Undefined();
}

Napi::Value set_controller_option(const Napi::CallbackInfo& info)
{
    CheckCount(info, 3);
    auto handle = ControllerInfo::FromValue(info[0])->handle;
    auto key = CheckAsString(info[1]);
    if (key == "ScreenshotTargetLongSide") {
        auto val = CheckAsNumber(info[2]).Int32Value();
        return Napi::Boolean::New(
            info.Env(),
            MaaControllerSetOption(
                handle,
                MaaCtrlOption_ScreenshotTargetLongSide,
                &val,
                sizeof(val)));
    }
    else if (key == "ScreenshotTargetShortSide") {
        auto val = CheckAsNumber(info[2]).Int32Value();
        return Napi::Boolean::New(
            info.Env(),
            MaaControllerSetOption(
                handle,
                MaaCtrlOption_ScreenshotTargetShortSide,
                &val,
                sizeof(val)));
    }
    else if (key == "DefaultAppPackageEntry") {
        auto val = CheckAsString(info[2]);
        return Napi::Boolean::New(
            info.Env(),
            MaaControllerSetOption(
                handle,
                MaaCtrlOption_DefaultAppPackageEntry,
                val.data(),
                val.length()));
    }
    else if (key == "DefaultAppPackage") {
        auto val = CheckAsString(info[2]);
        return Napi::Boolean::New(
            info.Env(),
            MaaControllerSetOption(
                handle,
                MaaCtrlOption_DefaultAppPackage,
                val.data(),
                val.length()));
    }
    else if (key == "Recording") {
        auto val = CheckAsBoolean(info[2]).Value();
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
    CheckCount(info, 1);
    auto handle = ControllerInfo::FromValue(info[0])->handle;
    auto ctrlId = MaaControllerPostConnection(handle);
    return Napi::Number::New(info.Env(), ctrlId);
}

Napi::Value controller_post_click(const Napi::CallbackInfo& info)
{
    CheckCount(info, 3);
    auto handle = ControllerInfo::FromValue(info[0])->handle;
    auto x = CheckAsNumber(info[1]).Int32Value();
    auto y = CheckAsNumber(info[2]).Int32Value();
    auto ctrlId = MaaControllerPostClick(handle, x, y);
    return Napi::Number::New(info.Env(), ctrlId);
}

Napi::Value controller_post_swipe(const Napi::CallbackInfo& info)
{
    CheckCount(info, 6);
    auto handle = ControllerInfo::FromValue(info[0])->handle;
    auto x1 = CheckAsNumber(info[1]).Int32Value();
    auto y1 = CheckAsNumber(info[2]).Int32Value();
    auto x2 = CheckAsNumber(info[3]).Int32Value();
    auto y2 = CheckAsNumber(info[4]).Int32Value();
    auto duration = CheckAsNumber(info[5]).Int32Value();
    auto ctrlId = MaaControllerPostSwipe(handle, x1, y1, x2, y2, duration);
    return Napi::Number::New(info.Env(), ctrlId);
}

Napi::Value controller_post_press_key(const Napi::CallbackInfo& info)
{
    CheckCount(info, 2);
    auto handle = ControllerInfo::FromValue(info[0])->handle;
    auto key = CheckAsNumber(info[1]).Int32Value();
    auto ctrlId = MaaControllerPostPressKey(handle, key);
    return Napi::Number::New(info.Env(), ctrlId);
}

Napi::Value controller_post_input_text(const Napi::CallbackInfo& info)
{
    CheckCount(info, 2);
    auto handle = ControllerInfo::FromValue(info[0])->handle;
    auto text = CheckAsString(info[1]);
    auto ctrlId = MaaControllerPostInputText(handle, text.c_str());
    return Napi::Number::New(info.Env(), ctrlId);
}

Napi::Value controller_post_start_app(const Napi::CallbackInfo& info)
{
    CheckCount(info, 2);
    auto handle = ControllerInfo::FromValue(info[0])->handle;
    auto intent = CheckAsString(info[1]);
    auto ctrlId = MaaControllerPostStartApp(handle, intent.c_str());
    return Napi::Number::New(info.Env(), ctrlId);
}

Napi::Value controller_post_stop_app(const Napi::CallbackInfo& info)
{
    CheckCount(info, 2);
    auto handle = ControllerInfo::FromValue(info[0])->handle;
    auto intent = CheckAsString(info[1]);
    auto ctrlId = MaaControllerPostStopApp(handle, intent.c_str());
    return Napi::Number::New(info.Env(), ctrlId);
}

Napi::Value controller_post_touch_down(const Napi::CallbackInfo& info)
{
    CheckCount(info, 5);
    auto handle = ControllerInfo::FromValue(info[0])->handle;
    auto contact = CheckAsNumber(info[1]).Int32Value();
    auto x = CheckAsNumber(info[2]).Int32Value();
    auto y = CheckAsNumber(info[3]).Int32Value();
    auto pressure = CheckAsNumber(info[4]).Int32Value();
    auto ctrlId = MaaControllerPostTouchDown(handle, contact, x, y, pressure);
    return Napi::Number::New(info.Env(), ctrlId);
}

Napi::Value controller_post_touch_move(const Napi::CallbackInfo& info)
{
    CheckCount(info, 5);
    auto handle = ControllerInfo::FromValue(info[0])->handle;
    auto contact = CheckAsNumber(info[1]).Int32Value();
    auto x = CheckAsNumber(info[2]).Int32Value();
    auto y = CheckAsNumber(info[3]).Int32Value();
    auto pressure = CheckAsNumber(info[4]).Int32Value();
    auto ctrlId = MaaControllerPostTouchMove(handle, contact, x, y, pressure);
    return Napi::Number::New(info.Env(), ctrlId);
}

Napi::Value controller_post_touch_up(const Napi::CallbackInfo& info)
{
    CheckCount(info, 2);
    auto handle = ControllerInfo::FromValue(info[0])->handle;
    auto contact = CheckAsNumber(info[1]).Int32Value();
    auto ctrlId = MaaControllerPostTouchUp(handle, contact);
    return Napi::Number::New(info.Env(), ctrlId);
}

Napi::Value controller_post_screencap(const Napi::CallbackInfo& info)
{
    CheckCount(info, 1);
    auto handle = ControllerInfo::FromValue(info[0])->handle;
    auto ctrlId = MaaControllerPostScreencap(handle);
    return Napi::Number::New(info.Env(), ctrlId);
}

Napi::Value controller_status(const Napi::CallbackInfo& info)
{
    CheckCount(info, 2);
    auto handle = ControllerInfo::FromValue(info[0])->handle;
    auto id = CheckAsNumber(info[1]).Uint32Value();
    return Napi::Number::New(info.Env(), MaaControllerStatus(handle, id));
}

Napi::Value controller_wait(const Napi::CallbackInfo& info)
{
    CheckCount(info, 2);
    auto handle = ControllerInfo::FromValue(info[0])->handle;
    auto id = CheckAsNumber(info[1]).Uint32Value();
    auto worker = new SimpleAsyncWork<MaaStatus>(
        info.Env(),
        [handle, id]() { return MaaControllerWait(handle, id); },
        [](auto env, auto res) { return Napi::Number::New(env, res); });
    worker->Queue();
    return worker->Promise();
}

Napi::Value controller_connected(const Napi::CallbackInfo& info)
{
    CheckCount(info, 1);
    auto handle = ControllerInfo::FromValue(info[0])->handle;
    return Napi::Boolean::New(info.Env(), MaaControllerConnected(handle));
}

Napi::Value controller_get_image(const Napi::CallbackInfo& info)
{
    CheckCount(info, 2);
    auto handle = ControllerInfo::FromValue(info[0])->handle;
    auto value = CheckAsExternal<MaaImageBuffer>(info[1]).Data();
    return Napi::Boolean::New(info.Env(), MaaControllerGetImage(handle, value));
}

Napi::Value controller_get_uuid(const Napi::CallbackInfo& info)
{
    CheckCount(info, 1);
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
    BIND(custom_controller_create);
    BIND(controller_destroy);
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
