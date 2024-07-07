#include "../include/helper.h"
#include "../include/loader.h"

#include <MaaFramework/MaaAPI.h>
#include <MaaToolkit/MaaToolkitAPI.h>

Napi::Value find_window(const Napi::CallbackInfo& info)
{
    auto class_name = CheckAsString(info[0]);
    auto window_name = CheckAsString(info[1]);
    return Napi::Number::New(
        info.Env(),
        MaaToolkitFindWindow(class_name.c_str(), window_name.c_str()));
}

Napi::Value search_window(const Napi::CallbackInfo& info)
{
    auto class_name = CheckAsString(info[0]);
    auto window_name = CheckAsString(info[1]);
    return Napi::Number::New(
        info.Env(),
        MaaToolkitSearchWindow(class_name.c_str(), window_name.c_str()));
}

Napi::Value list_windows(const Napi::CallbackInfo& info)
{
    return Napi::Number::New(info.Env(), MaaToolkitListWindows());
}

Napi::Value get_window(const Napi::CallbackInfo& info)
{
    auto index = CheckAsNumber(info[0]).Uint32Value();
    return Napi::External<void>::New(info.Env(), MaaToolkitGetWindow(index));
}

Napi::Value get_cursor_window(const Napi::CallbackInfo& info)
{
    return Napi::External<void>::New(info.Env(), MaaToolkitGetCursorWindow());
}

Napi::Value get_desktop_window(const Napi::CallbackInfo& info)
{
    return Napi::External<void>::New(info.Env(), MaaToolkitGetDesktopWindow());
}

Napi::Value get_foreground_window(const Napi::CallbackInfo& info)
{
    return Napi::External<void>::New(info.Env(), MaaToolkitGetForegroundWindow());
}

Napi::Value get_window_info(const Napi::CallbackInfo& info)
{
    MaaWin32Hwnd hwnd = CheckAsExternal<void>(info[0]).Data();
    StringBuffer class_name, window_name;
    auto result = Napi::Object::New(info.Env());
    if (MaaToolkitGetWindowClassName(hwnd, class_name)) {
        result["class_name"] = class_name;
    }
    if (MaaToolkitGetWindowWindowName(hwnd, window_name)) {
        result["window_name"] = window_name;
    }
    return result;
}

void load_win32_win32Window(Napi::Env env, Napi::Object& exports)
{
    BIND(find_window);
    BIND(search_window);
    BIND(list_windows);
    BIND(get_window);
    BIND(get_cursor_window);
    BIND(get_desktop_window);
    BIND(get_foreground_window);
    BIND(get_window_info);
}
