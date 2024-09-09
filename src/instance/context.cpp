#include "../include/info.h"
#include "../include/loader.h"
#include "../include/wrapper.h"

#include <MaaFramework/MaaAPI.h>

MaaTaskId
    context_run_pipeline(Napi::External<MaaContext> info, std::string entry, std::string overr)
{
    return MaaContextRunPipeline(info.Data(), entry.c_str(), overr.c_str());
}

MaaRecoId context_run_recognition(
    Napi::External<MaaContext> info,
    std::string entry,
    std::string overr,
    Napi::ArrayBuffer image_buf)
{
    ImageBuffer image;
    image.set(image_buf);
    return MaaContextRunRecognition(info.Data(), entry.c_str(), overr.c_str(), image);
}

MaaNodeId context_run_action(
    Napi::External<MaaContext> info,
    std::string entry,
    std::string overr,
    MaaRect box,
    std::string detail)
{
    return MaaContextRunAction(info.Data(), entry.c_str(), overr.c_str(), &box, detail.c_str());
}

MaaNodeId context_override_pipeline(Napi::External<MaaContext> info, std::string overr)
{
    return MaaContextOverridePipeline(info.Data(), overr.c_str());
}

MaaTaskId context_get_task_id(Napi::External<MaaContext> info)
{
    return MaaContextGetTaskId(info.Data());
}

std::optional<Napi::External<TaskerInfo>>
    context_get_tasker(ExtContextInfo* context, Napi::External<MaaContext> info)
{
    auto tasker = MaaContextGetTasker(info.Data());
    if (!tasker) {
        return std::nullopt;
    }
    auto it = context->taskers.find(tasker);
    if (it == context->taskers.end()) {
        return std::nullopt;
    }
    // 这里如果弱引用失效了会抛异常, 但是这种情况下确实不正常了, 直接抛也可以
    return it->second.Value();
}

Napi::External<MaaContext> context_clone(Napi::External<MaaContext> info)
{
    return Napi::External<MaaContext>::New(info.Env(), MaaContextClone(info.Data()));
}

void load_instance_context(
    Napi::Env env,
    Napi::Object& exports,
    Napi::External<ExtContextInfo> context)
{
    BIND(context_run_pipeline);
    BIND(context_run_recognition);
    BIND(context_run_action);
    BIND(context_override_pipeline);
    BIND(context_get_task_id);
    // BIND(context_get_tasker);
    BIND(context_clone);
}
