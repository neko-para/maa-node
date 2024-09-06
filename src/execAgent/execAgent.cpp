#include "../include/helper.h"
#include "../include/info.h"
#include "../include/loader.h"

#include <MaaFramework/MaaAPI.h>
#include <MaaToolkit/MaaToolkitAPI.h>

#include <algorithm>
#include <iterator>

static inline std::vector<std::string> ArrayToStringList(Napi::Array arr)
{
    std::vector<std::string> result;
    result.reserve(arr.Length());
    for (auto [prop, val] : arr) {
        result.push_back(CheckAsString(val));
    }
    return result;
}

Napi::Value register_custom_recognizer_executor(const Napi::CallbackInfo& info)
{
    CheckCount(info, 4);
    auto handle = TaskerInfo::HandleFromValue(info[0]);
    auto recognizer_name = CheckAsString(info[1]);
    auto exec_path = CheckAsString(info[2]);
    auto exec_params = ArrayToStringList(CheckAsArray(info[3]));
    std::vector<MaaStringView> exec_params_view;
    exec_params_view.reserve(exec_params.size());
    std::transform(
        exec_params.begin(),
        exec_params.end(),
        std::back_insert_iterator<std::vector<MaaStringView>>(exec_params_view),
        [](const auto& str) { return str.c_str(); });
    return Napi::Boolean::New(
        info.Env(),
        MaaToolkitRegisterCustomRecognizerExecutor(
            handle,
            recognizer_name.c_str(),
            exec_path.c_str(),
            exec_params_view.data(),
            exec_params_view.size()));
}

Napi::Value unregister_custom_recognizer_executor(const Napi::CallbackInfo& info)
{
    CheckCount(info, 2);
    auto handle = TaskerInfo::HandleFromValue(info[0]);
    auto recognizer_name = CheckAsString(info[1]);
    return Napi::Boolean::New(
        info.Env(),
        MaaToolkitUnregisterCustomRecognizerExecutor(handle, recognizer_name.c_str()));
}

Napi::Value clear_custom_recognizer_executor(const Napi::CallbackInfo& info)
{
    CheckCount(info, 1);
    auto handle = TaskerInfo::HandleFromValue(info[0]);
    return Napi::Boolean::New(info.Env(), MaaToolkitClearCustomRecognizerExecutor(handle));
}

Napi::Value register_custom_action_executor(const Napi::CallbackInfo& info)
{
    CheckCount(info, 4);
    auto handle = TaskerInfo::HandleFromValue(info[0]);
    auto action_name = CheckAsString(info[1]);
    auto exec_path = CheckAsString(info[2]);
    auto exec_params = ArrayToStringList(CheckAsArray(info[3]));
    std::vector<MaaStringView> exec_params_view;
    exec_params_view.reserve(exec_params.size());
    std::transform(
        exec_params.begin(),
        exec_params.end(),
        std::back_insert_iterator<std::vector<MaaStringView>>(exec_params_view),
        [](const auto& str) { return str.c_str(); });
    return Napi::Boolean::New(
        info.Env(),
        MaaToolkitRegisterCustomActionExecutor(
            handle,
            action_name.c_str(),
            exec_path.c_str(),
            exec_params_view.data(),
            exec_params_view.size()));
}

Napi::Value unregister_custom_action_executor(const Napi::CallbackInfo& info)
{
    CheckCount(info, 2);
    auto handle = TaskerInfo::HandleFromValue(info[0]);
    auto action_name = CheckAsString(info[1]);
    return Napi::Boolean::New(
        info.Env(),
        MaaToolkitUnregisterCustomActionExecutor(handle, action_name.c_str()));
}

Napi::Value clear_custom_action_executor(const Napi::CallbackInfo& info)
{
    CheckCount(info, 1);
    auto handle = TaskerInfo::HandleFromValue(info[0]);
    return Napi::Boolean::New(info.Env(), MaaToolkitClearCustomActionExecutor(handle));
}

void load_execAgent_execAgent(Napi::Env env, Napi::Object& exports)
{
    BIND(register_custom_recognizer_executor);
    BIND(unregister_custom_recognizer_executor);
    BIND(clear_custom_recognizer_executor);
    BIND(register_custom_action_executor);
    BIND(unregister_custom_action_executor);
    BIND(clear_custom_action_executor);
}
