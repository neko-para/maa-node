#include "include/loader.h"

#include <MaaFramework/MaaAPI.h>
#include <napi.h>

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    Napi::Value a = exports;
    load_instance_controller(env, exports);
    load_instance_instance(env, exports);
    load_instance_resource(env, exports);
    load_task_syncContext(env, exports);
    load_utility_buffer(env, exports);
    load_utility_utility(env, exports);
    load_config_config(env, exports);
    load_device_device(env, exports);
    load_execAgent_execAgent(env, exports);
    load_win32_win32Window(env, exports);

#define DE(prefix, key) prefix[#key] = Napi::Number::New(env, prefix##_##key)
#define DEN(prefix, key, name) prefix[name] = Napi::Number::New(env, prefix##_##key)

    auto MaaStatus = Napi::Object::New(env);
    DE(MaaStatus, Invalid);
    DE(MaaStatus, Pending);
    DE(MaaStatus, Running);
    DE(MaaStatus, Success);
    DE(MaaStatus, Failed);
    exports["Status"] = MaaStatus;

    auto MaaLoggingLevel = Napi::Object::New(env);
    DE(MaaLoggingLevel, Off);
    DE(MaaLoggingLevel, Fatal);
    DE(MaaLoggingLevel, Error);
    DE(MaaLoggingLevel, Warn);
    DE(MaaLoggingLevel, Info);
    DE(MaaLoggingLevel, Debug);
    DE(MaaLoggingLevel, Trace);
    DE(MaaLoggingLevel, All);
    exports["LoggingLevel"] = MaaLoggingLevel;

    auto MaaAdbControllerType = Napi::Object::New(env);
    DE(MaaAdbControllerType, Touch_Adb);
    DE(MaaAdbControllerType, Touch_MiniTouch);
    DE(MaaAdbControllerType, Touch_MaaTouch);
    DE(MaaAdbControllerType, Touch_EmulatorExtras);
    DE(MaaAdbControllerType, Touch_AutoDetect);
    DE(MaaAdbControllerType, Key_Adb);
    DE(MaaAdbControllerType, Key_MaaTouch);
    DE(MaaAdbControllerType, Key_EmulatorExtras);
    DE(MaaAdbControllerType, Key_AutoDetect);
    DE(MaaAdbControllerType, Input_Preset_Adb);
    DEN(MaaAdbControllerType, Input_Preset_Minitouch, "Input_Preset_MiniTouch");
    DEN(MaaAdbControllerType, Input_Preset_Maatouch, "Input_Preset_MaaTouch");
    DE(MaaAdbControllerType, Input_Preset_AutoDetect);
    DE(MaaAdbControllerType, Input_Preset_EmulatorExtras);
    DE(MaaAdbControllerType, Screencap_FastestWay_Compatible);
    DE(MaaAdbControllerType, Screencap_RawByNetcat);
    DE(MaaAdbControllerType, Screencap_RawWithGzip);
    DE(MaaAdbControllerType, Screencap_Encode);
    DE(MaaAdbControllerType, Screencap_EncodeToFile);
    DE(MaaAdbControllerType, Screencap_MinicapDirect);
    DE(MaaAdbControllerType, Screencap_MinicapStream);
    DE(MaaAdbControllerType, Screencap_EmulatorExtras);
    DE(MaaAdbControllerType, Screencap_FastestLosslessWay);
    DE(MaaAdbControllerType, Screencap_FastestWay);
    exports["MaaAdbControllerType"] = MaaAdbControllerType;

    auto MaaWin32ControllerType = Napi::Object::New(env);
    DE(MaaWin32ControllerType, Touch_SendMessage);
    DE(MaaWin32ControllerType, Touch_Seize);
    DE(MaaWin32ControllerType, Key_SendMessage);
    DE(MaaWin32ControllerType, Key_Seize);
    DE(MaaWin32ControllerType, Screencap_GDI);
    DE(MaaWin32ControllerType, Screencap_DXGI_DesktopDup);
    DE(MaaWin32ControllerType, Screencap_DXGI_FramePool);
    exports["Win32ControllerType"] = MaaWin32ControllerType;

    return exports;
}

NODE_API_MODULE(maa, Init)
