#include "../include/loader.h"

#include <MaaFramework/MaaAPI.h>
#include <napi.h>

#include <iostream>

void ImageBufferFinalizer(Napi::Env, MaaImageBufferHandle handle)
{
    std::cerr << "destroy image" << std::endl;
    MaaDestroyImageBuffer(handle);
}

Napi::Value create_image_buffer(const Napi::CallbackInfo& info)
{
    auto handle = MaaCreateImageBuffer();
    if (handle) {
        return Napi::External<MaaImageBuffer>::New(info.Env(), handle, ImageBufferFinalizer);
    }
    else {
        return info.Env().Null();
    }
}

Napi::Value is_image_empty(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaImageBuffer>>();
    return Napi::Boolean::New(info.Env(), MaaIsImageEmpty(handle.Data()) > 0);
}

Napi::Value clear_image(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaImageBuffer>>();
    return Napi::Boolean::New(info.Env(), MaaClearImage(handle.Data()) > 0);
}

void load_utility_buffer(Napi::Env env, Napi::Object& exports)
{
    exports["create_image_buffer"] =
        Napi::Function::New(env, create_image_buffer, "MaaCreateImageBuffer");
    exports["is_image_empty"] = Napi::Function::New(env, is_image_empty, "MaaIsImageEmpty");
    exports["clear_image"] = Napi::Function::New(env, clear_image, "MaaClearImage");
}
