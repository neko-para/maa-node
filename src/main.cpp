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

    return exports;
}

NODE_API_MODULE(maa, Init)
