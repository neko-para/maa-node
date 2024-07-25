#include "../include/helper.h"
#include "../include/loader.h"

#include <MaaFramework/MaaAPI.h>
#include <MaaToolkit/MaaToolkitAPI.h>
#include <charconv>
#include <cstdint>
#include <string>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

Napi::Value find_window(const Napi::CallbackInfo& info)
{
    CheckCount(info, 2);
    auto class_name = CheckAsString(info[0]);
    auto window_name = CheckAsString(info[1]);
    return Napi::Number::New(
        info.Env(),
        MaaToolkitFindWindow(class_name.c_str(), window_name.c_str()));
}

Napi::Value search_window(const Napi::CallbackInfo& info)
{
    CheckCount(info, 2);
    auto class_name = CheckAsString(info[0]);
    auto window_name = CheckAsString(info[1]);
    return Napi::Number::New(
        info.Env(),
        MaaToolkitSearchWindow(class_name.c_str(), window_name.c_str()));
}

Napi::Value list_windows(const Napi::CallbackInfo& info)
{
    CheckCount(info, 0);
    return Napi::Number::New(info.Env(), MaaToolkitListWindows());
}

Napi::Value get_window(const Napi::CallbackInfo& info)
{
    CheckCount(info, 1);
    auto index = CheckAsNumber(info[0]).Uint32Value();
    return Napi::External<void>::New(info.Env(), MaaToolkitGetWindow(index));
}

Napi::Value get_cursor_window(const Napi::CallbackInfo& info)
{
    CheckCount(info, 0);
    return Napi::External<void>::New(info.Env(), MaaToolkitGetCursorWindow());
}

Napi::Value get_desktop_window(const Napi::CallbackInfo& info)
{
    CheckCount(info, 0);
    return Napi::External<void>::New(info.Env(), MaaToolkitGetDesktopWindow());
}

Napi::Value get_foreground_window(const Napi::CallbackInfo& info)
{
    CheckCount(info, 0);
    return Napi::External<void>::New(info.Env(), MaaToolkitGetForegroundWindow());
}

Napi::Value get_window_info(const Napi::CallbackInfo& info)
{
    CheckCount(info, 1);
    MaaWin32Hwnd hwnd = CheckAsExternal<void>(info[0]).Data();
    StringBuffer class_name, window_name;
    auto result = Napi::Object::New(info.Env());
    if (MaaToolkitGetWindowClassName(hwnd, class_name)) {
        result["class_name"] = class_name.str();
    }
    if (MaaToolkitGetWindowWindowName(hwnd, window_name)) {
        result["window_name"] = window_name.str();
    }
    return result;
}

Napi::Value unwrap_window_hwnd(const Napi::CallbackInfo& info)
{
    CheckCount(info, 1);
    MaaWin32Hwnd hwnd = CheckAsExternal<void>(info[0]).Data();
    return Napi::String::New(info.Env(), fmt::format("{:16x}", reinterpret_cast<size_t>(hwnd)));
}

Napi::Value wrap_window_hwnd(const Napi::CallbackInfo& info)
{
    CheckCount(info, 1);
#ifdef _WIN32
    auto hwnd_str = CheckAsString(info[0]);
    uint64_t hwnd_ptr = 0;
    std::from_chars(hwnd_str.c_str(), hwnd_str.c_str() + hwnd_str.size(), hwnd_ptr, 16);
    auto hwnd = reinterpret_cast<MaaWin32Hwnd>(hwnd_ptr);
    if (IsWindow(reinterpret_cast<HWND>(hwnd))) {
        return Napi::External<void>::New(info.Env(), hwnd);
    }
    else {
        return info.Env().Null();
    }
#else
    return info.Env().Null();
#endif
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
    BIND(unwrap_window_hwnd);
    BIND(wrap_window_hwnd);
}
