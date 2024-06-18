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
    auto handle = info[0].As<Napi::External<MaaImageBuffer>>().Data();
    return Napi::Boolean::New(info.Env(), MaaIsImageEmpty(handle));
}

Napi::Value clear_image(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaImageBuffer>>().Data();
    return Napi::Boolean::New(info.Env(), MaaClearImage(handle));
}

Napi::Value get_image_info(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaImageBuffer>>();
    auto result = Napi::Object::New(info.Env());
    result["width"] = MaaGetImageWidth(handle.Data());
    result["height"] = MaaGetImageHeight(handle.Data());
    result["type"] = MaaGetImageType(handle.Data());
    return result;
}

Napi::Value get_image_encoded(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaImageBuffer>>().Data();
    return Napi::ArrayBuffer::New(
        info.Env(),
        MaaGetImageEncoded(handle),
        MaaGetImageEncodedSize(handle));
}

Napi::Value set_image_encoded(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaImageBuffer>>().Data();
    auto data = info[1].As<Napi::ArrayBuffer>();
    return Napi::Boolean::New(
        info.Env(),
        MaaSetImageEncoded(handle, reinterpret_cast<uint8_t*>(data.Data()), data.ByteLength()));
}

void ImageListBufferFinalizer(Napi::Env, MaaImageListBufferHandle handle)
{
    std::cerr << "destroy image list" << std::endl;
    MaaDestroyImageListBuffer(handle);
}

Napi::Value create_image_list_buffer(const Napi::CallbackInfo& info)
{
    auto handle = MaaCreateImageListBuffer();
    if (handle) {
        return Napi::External<MaaImageListBuffer>::New(
            info.Env(),
            handle,
            ImageListBufferFinalizer);
    }
    else {
        return info.Env().Null();
    }
}

Napi::Value is_image_list_empty(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaImageListBuffer>>().Data();
    return Napi::Boolean::New(info.Env(), MaaIsImageListEmpty(handle));
}

Napi::Value clear_image_list(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaImageListBuffer>>().Data();
    return Napi::Boolean::New(info.Env(), MaaClearImageList(handle));
}

Napi::Value get_image_list_size(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaImageListBuffer>>().Data();
    return Napi::Number::New(info.Env(), MaaGetImageListSize(handle));
}

Napi::Value get_image_list_at(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaImageListBuffer>>().Data();
    auto index = info[1].As<Napi::Number>().Uint32Value();
    return Napi::External<MaaImageBuffer>::New(info.Env(), MaaGetImageListAt(handle, index));
}

Napi::Value image_list_append(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaImageListBuffer>>().Data();
    auto value = info[1].As<Napi::External<MaaImageBuffer>>().Data();
    return Napi::Boolean::New(info.Env(), MaaImageListAppend(handle, value));
}

Napi::Value image_list_remove(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaImageListBuffer>>().Data();
    auto index = info[1].As<Napi::Number>().Uint32Value();
    return Napi::Boolean::New(info.Env(), MaaImageListRemove(handle, index));
}

void load_utility_buffer(Napi::Env env, Napi::Object& exports)
{
    exports["create_image_buffer"] =
        Napi::Function::New(env, create_image_buffer, "MaaCreateImageBuffer");
    exports["is_image_empty"] = Napi::Function::New(env, is_image_empty, "MaaIsImageEmpty");
    exports["clear_image"] = Napi::Function::New(env, clear_image, "MaaClearImage");
    exports["get_image_info"] = Napi::Function::New(env, get_image_info, "MaaGetImageXXX");
    exports["get_image_encoded"] =
        Napi::Function::New(env, get_image_encoded, "MaaGetImageEncoded");
    exports["set_image_encoded"] =
        Napi::Function::New(env, set_image_encoded, "MaaSetImageEncoded");

    exports["create_image_list_buffer"] =
        Napi::Function::New(env, create_image_list_buffer, "MaaCreateImageListBuffer");
    exports["is_image_list_empty"] =
        Napi::Function::New(env, is_image_list_empty, "MaaIsImageListEmpty");
    exports["clear_image_list"] = Napi::Function::New(env, clear_image_list, "MaaClearImageList");
    exports["get_image_list_size"] =
        Napi::Function::New(env, get_image_list_size, "MaaGetImageListSize");
    exports["get_image_list_at"] = Napi::Function::New(env, get_image_list_at, "MaaGetImageListAt");
    exports["image_list_append"] =
        Napi::Function::New(env, image_list_append, "MaaImageListAppend");
    exports["image_list_remove"] =
        Napi::Function::New(env, image_list_remove, "MaaImageListRemove");
}
