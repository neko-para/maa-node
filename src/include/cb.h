#pragma once

#include "info.h"
#include "utils.h"
#include "wrapper.h"

#include <iostream>

inline void NotificationCallback(const char* message, const char* details_json, void* callback_arg)
{
    auto ctx = reinterpret_cast<CallbackContext*>(callback_arg);
    ctx->Call<void>(
        [=](auto env, auto fn) {
            return fn.Call(
                { Napi::String::New(env, message), Napi::String::New(env, details_json) });
        },
        [](auto res) { std::ignore = res; });
}

inline MaaBool CustomRecognizerCallback(
    MaaContext* context,
    MaaTaskId task_id,
    const char* current_task_name,
    const char* custom_recognition_name,
    const char* custom_recognition_param,
    const MaaImageBuffer* image,
    const MaaRect* roi,
    void* trans_arg,
    MaaRect* out_box,
    MaaStringBuffer* out_detail)
{
    auto ctx = reinterpret_cast<CallbackContext*>(trans_arg);
    using Result = std::optional<std::tuple<MaaRect, std::string>>;
    auto res = ctx->Call<Result>(
        [=](auto env, auto fn) {
            return fn.Call({
                Napi::External<MaaContext>::New(env, context),
                JSConvert<MaaTaskId>::to_value(env, task_id),
                Napi::String::New(env, current_task_name),
                Napi::String::New(env, custom_recognition_name),
                Napi::String::New(env, custom_recognition_param),
                ImageBufferRefer(image).data(env),
                JSConvert<MaaRect>::to_value(env, *roi),
            });
        },
        [](Napi::Value res) -> Result {
            try {
                return JSConvert<Result>::from_value(res);
            }
            catch (MaaNodeException exc) {
                std::cerr << exc.what() << std::endl;
                return std::nullopt;
            }
        });

    if (res.has_value()) {
        *out_box = std::get<0>(res.value());
        StringBuffer(out_detail, false).set(std::get<1>(res.value()));
        return true;
    }
    else {
        return false;
    }
}

inline MaaBool CustomActionCallback(
    MaaContext* context,
    MaaTaskId task_id,
    const char* current_task_name,
    const char* custom_action_name,
    const char* custom_action_param,
    MaaRecoId reco_id,
    const MaaRect* box,
    void* trans_arg)
{
    auto ctx = reinterpret_cast<CallbackContext*>(trans_arg);

    auto res = ctx->Call<bool>(
        [=](auto env, auto fn) {
            return fn.Call({
                Napi::External<MaaContext>::New(env, context),
                JSConvert<MaaTaskId>::to_value(env, task_id),
                Napi::String::New(env, current_task_name),
                Napi::String::New(env, custom_action_name),
                Napi::String::New(env, custom_action_param),
                JSConvert<MaaRecoId>::to_value(env, reco_id),
                JSConvert<MaaRect>::to_value(env, *box),
            });
        },
        [](Napi::Value res) -> bool {
            try {
                return JSConvert<bool>::from_value(res);
            }
            catch (MaaNodeException exc) {
                std::cerr << exc.what() << std::endl;
                return false;
            }
        });

    return res;
}
