#include "../include/helper.h"
#include "../include/loader.h"

#include <MaaFramework/MaaAPI.h>
#include <cstdint>
#include <napi.h>

Napi::Value version(const Napi::CallbackInfo& info)
{
    CheckCount(info, 0);
    return Napi::String::New(info.Env(), MaaVersion());
}

Napi::Value set_global_option(const Napi::CallbackInfo& info)
{
    CheckCount(info, 2);
    auto key = CheckAsString(info[0]);
    if (key == "LogDir") {
        auto val = CheckAsString(info[1]);
        return Napi::Boolean::New(
            info.Env(),
            MaaSetGlobalOption(MaaGlobalOption_LogDir, val.data(), val.length()));
    }
    else if (key == "SaveDraw") {
        auto val = CheckAsBoolean(info[1]).Value();
        return Napi::Boolean::New(
            info.Env(),
            MaaSetGlobalOption(MaaGlobalOption_SaveDraw, &val, sizeof(val)));
    }
    else if (key == "Recording") {
        auto val = CheckAsBoolean(info[1]).Value();
        return Napi::Boolean::New(
            info.Env(),
            MaaSetGlobalOption(MaaGlobalOption_Recording, &val, sizeof(val)));
    }
    else if (key == "StdoutLevel") {
        auto val = CheckAsNumber(info[1]).Int32Value();
        return Napi::Boolean::New(
            info.Env(),
            MaaSetGlobalOption(MaaGlobalOption_StdoutLevel, &val, sizeof(val)));
    }
    else if (key == "ShowHitDraw") {
        auto val = CheckAsBoolean(info[1]).Value();
        return Napi::Boolean::New(
            info.Env(),
            MaaSetGlobalOption(MaaGlobalOption_ShowHitDraw, &val, sizeof(val)));
    }
    else if (key == "DebugMessage") {
        auto val = CheckAsBoolean(info[1]).Value();
        return Napi::Boolean::New(
            info.Env(),
            MaaSetGlobalOption(MaaGlobalOption_DebugMessage, &val, sizeof(val)));
    }
    else {
        return Napi::Boolean::New(info.Env(), false);
    }
}

Napi::Value query_recognition_detail(const Napi::CallbackInfo& info)
{
    CheckCount(info, 1);
    MaaRecoId reco_id = CheckAsNumber(info[0]).Int64Value();
    StringBuffer name;
    MaaBool hit;
    MaaRect hit_box;
    StringBuffer detail_json;
    auto raw = ExternalOrNull<MaaImageBuffer>(info[1]);
    auto draws = ExternalOrNull<MaaImageListBuffer>(info[2]);
    auto ret = MaaQueryRecognitionDetail(
        reco_id,
        name,
        &hit,
        &hit_box,
        detail_json,
        raw.value_or(nullptr),
        draws.value_or(nullptr));
    if (ret) {
        auto result = Napi::Object::New(info.Env());
        result["name"] = name;
        result["hit"] = !!hit;
        result["hit_box"] = FromRect(info.Env(), hit_box);
        result["detail_json"] = detail_json;
        return result;
    }
    else {
        return info.Env().Null();
    }
}

Napi::Value query_node_detail(const Napi::CallbackInfo& info)
{
    CheckCount(info, 1);
    MaaNodeId node_id = CheckAsNumber(info[0]).Int64Value();
    StringBuffer name;
    MaaRecoId reco_id;
    MaaBool run_completed;
    auto ret = MaaQueryNodeDetail(node_id, name, &reco_id, &run_completed);
    if (ret) {
        auto result = Napi::Object::New(info.Env());
        result["name"] = name;
        result["reco_id"] = reco_id;
        result["run_completed"] = !!run_completed;
        return result;
    }
    else {
        return info.Env().Null();
    }
}

Napi::Value query_task_detail(const Napi::CallbackInfo& info)
{
    CheckCount(info, 1);
    MaaTaskId task_id = CheckAsNumber(info[0]).Int64Value();
    MaaSize node_id_list_size = 0;
    auto ret = MaaQueryTaskDetail(task_id, nullptr, nullptr, &node_id_list_size);
    if (!ret) {
        return info.Env().Null();
    }
    std::vector<MaaNodeId> node_id_list(node_id_list_size);
    StringBuffer entry;
    ret = MaaQueryTaskDetail(task_id, entry, node_id_list.data(), &node_id_list_size);
    if (ret) {
        auto result = Napi::Object::New(info.Env());
        result["entry"] = entry;
        auto list = Napi::Array::New(info.Env(), node_id_list_size);
        for (MaaSize i = 0; i < node_id_list_size; i++) {
            list.Set(static_cast<uint32_t>(i), Napi::Number::New(info.Env(), node_id_list[i]));
        }
        result["node_id_list"] = list;
        return result;
    }
    else {
        return info.Env().Null();
    }
}

void load_utility_utility(Napi::Env env, Napi::Object& exports)
{
    BIND(version);
    BIND(set_global_option);
    BIND(query_recognition_detail);
    BIND(query_node_detail);
    BIND(query_task_detail);
}
