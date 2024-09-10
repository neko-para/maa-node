#include "../include/cb.h"
#include "../include/info.h"
#include "../include/loader.h"
#include "../include/utils.h"
#include "../include/wrapper.h"

#include <MaaFramework/MaaAPI.h>

std::optional<Napi::External<ControllerInfo>> adb_controller_create(
    Napi::Env env,
    std::string adb_path,
    std::string address,
    MaaAdbScreencapMethod screencap_methods,
    MaaAdbInputMethod input_methods,
    std::string config,
    std::string agent_path,
    std::optional<Napi::Function> callback)
{
    MaaNotificationCallback cb = nullptr;
    CallbackContext* ctx = nullptr;
    MaaController* handle = nullptr;

    if (callback) {
        cb = NotificationCallback;
        ctx = new CallbackContext { env, callback.value(), "NotificationCallback" };
    }

    handle = MaaAdbControllerCreate(
        adb_path.c_str(),
        address.c_str(),
        screencap_methods,
        input_methods,
        config.c_str(),
        agent_path.c_str(),
        cb,
        ctx);

    if (handle) {
        return Napi::External<ControllerInfo>::New(
            env,
            new ControllerInfo { handle, ctx },
            &DeleteFinalizer<ControllerInfo*>);
    }
    else {
        delete ctx;
        return std::nullopt;
    }
}

std::optional<Napi::External<ControllerInfo>> win32_controller_create(
    Napi::Env env,
    Napi::External<void> hwnd,
    MaaWin32ScreencapMethod screencap_methods,
    MaaWin32InputMethod input_methods,
    std::optional<Napi::Function> callback)
{
    MaaNotificationCallback cb = nullptr;
    CallbackContext* ctx = nullptr;
    MaaController* handle = nullptr;

    if (callback) {
        cb = NotificationCallback;
        ctx = new CallbackContext { env, callback.value(), "NotificationCallback" };
    }

    handle = MaaWin32ControllerCreate(hwnd.Data(), screencap_methods, input_methods, cb, ctx);

    if (handle) {
        return Napi::External<ControllerInfo>::New(
            env,
            new ControllerInfo { handle, ctx },
            &DeleteFinalizer<ControllerInfo*>);
    }
    else {
        delete ctx;
        return std::nullopt;
    }
}

void controller_destroy(Napi::External<ControllerInfo> info)
{
    info.Data()->dispose();
}

bool controller_set_option_screenshot_target_long_side(
    Napi::External<ControllerInfo> info,
    int32_t size)
{
    return MaaControllerSetOption(
        info.Data()->handle,
        MaaCtrlOption_ScreenshotTargetLongSide,
        &size,
        sizeof(size));
}

bool controller_set_option_screenshot_target_short_side(
    Napi::External<ControllerInfo> info,
    int32_t size)
{
    return MaaControllerSetOption(
        info.Data()->handle,
        MaaCtrlOption_ScreenshotTargetShortSide,
        &size,
        sizeof(size));
}

bool controller_set_option_recording(Napi::External<ControllerInfo> info, bool flag)
{
    return MaaControllerSetOption(
        info.Data()->handle,
        MaaCtrlOption_Recording,
        &flag,
        sizeof(flag));
}

MaaCtrlId controller_post_connection(Napi::External<ControllerInfo> info)
{
    return MaaControllerPostConnection(info.Data()->handle);
}

MaaCtrlId controller_post_screencap(Napi::External<ControllerInfo> info)
{
    return MaaControllerPostScreencap(info.Data()->handle);
}

MaaStatus controller_status(Napi::External<ControllerInfo> info, MaaCtrlId id)
{
    return MaaControllerStatus(info.Data()->handle, id);
}

Napi::Promise controller_wait(Napi::Env env, Napi::External<ControllerInfo> info, MaaCtrlId id)
{
    auto handle = info.Data()->handle;
    auto worker = new SimpleAsyncWork<MaaStatus, "controller_wait">(env, [handle, id]() {
        return MaaControllerWait(handle, id);
    });
    worker->Queue();
    return worker->Promise();
}

bool controller_connected(Napi::External<ControllerInfo> info)
{
    return MaaControllerConnected(info.Data()->handle);
}

std::optional<Napi::ArrayBuffer>
    controller_cached_image(Napi::Env env, Napi::External<ControllerInfo> info)
{
    ImageBuffer buffer;
    auto ret = MaaControllerCachedImage(info.Data()->handle, buffer);
    if (ret) {
        return buffer.data(env);
    }
    else {
        return std::nullopt;
    }
}

std::optional<std::string> controller_get_uuid(Napi::External<ControllerInfo> info)
{
    StringBuffer buffer;
    auto ret = MaaControllerGetUuid(info.Data()->handle, buffer);
    if (ret) {
        return Napi::String::New(info.Env(), buffer.str());
    }
    else {
        return std::nullopt;
    }
}

/*
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

Napi::Value controller_get_image(const Napi::CallbackInfo& info)
{
    CheckCount(info, 2);
    auto handle = ControllerInfo::FromValue(info[0])->handle;
    auto value = CheckAsExternal<MaaImageBuffer>(info[1]).Data();
    return Napi::Boolean::New(info.Env(), MaaControllerGetImage(handle, value));
}
*/

void load_instance_controller(
    Napi::Env env,
    Napi::Object& exports,
    Napi::External<ExtContextInfo> context)
{
    BIND(adb_controller_create);
    BIND(win32_controller_create);
    // BIND(custom_controller_create);
    BIND(controller_destroy);
    BIND(controller_set_option_screenshot_target_long_side);
    BIND(controller_set_option_screenshot_target_short_side);
    BIND(controller_set_option_recording);
    BIND(controller_post_connection);
    // BIND(controller_post_click);
    // BIND(controller_post_swipe);
    // BIND(controller_post_press_key);
    // BIND(controller_post_input_text);
    // BIND(controller_post_start_app);
    // BIND(controller_post_stop_app);
    // BIND(controller_post_touch_down);
    // BIND(controller_post_touch_move);
    // BIND(controller_post_touch_up);
    BIND(controller_post_screencap);
    BIND(controller_status);
    BIND(controller_wait);
    BIND(controller_connected);
    BIND(controller_cached_image);
    BIND(controller_get_uuid);
}
