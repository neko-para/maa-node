#include "../include/helper.h"
#include "../include/loader.h"

#include <MaaFramework/MaaAPI.h>
#include <cstdint>
#include <cstring>
#include <napi.h>

void ImageBufferFinalizer(Napi::Env, MaaImageBufferHandle handle)
{
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

Napi::Value get_image_encoded_copied(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaImageBuffer>>().Data();
    auto length = MaaGetImageEncodedSize(handle);
    auto buffer = Napi::ArrayBuffer::New(info.Env(), length);
    std::memcpy(buffer.Data(), MaaGetImageEncoded(handle), length);
    return buffer;
}

Napi::Value set_image_encoded(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaImageBuffer>>().Data();
    if (info[1].IsArrayBuffer()) {
        auto data = info[1].As<Napi::ArrayBuffer>();
        return Napi::Boolean::New(
            info.Env(),
            MaaSetImageEncoded(handle, reinterpret_cast<uint8_t*>(data.Data()), data.ByteLength()));
    }
    else if (info[1].IsBuffer()) {
        auto data = info[1].As<Napi::Buffer<uint8_t>>();
        return Napi::Boolean::New(
            info.Env(),
            MaaSetImageEncoded(handle, reinterpret_cast<uint8_t*>(data.Data()), data.ByteLength()));
    }
    else {
        return Napi::Boolean::New(info.Env(), false);
    }
}

void ImageListBufferFinalizer(Napi::Env, MaaImageListBufferHandle handle)
{
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
    BIND(create_image_buffer);
    BIND(is_image_empty);
    BIND(clear_image);
    BIND(get_image_info);
    BIND(get_image_encoded);
    BIND(get_image_encoded_copied);
    BIND(set_image_encoded);

    BIND(create_image_list_buffer);
    BIND(is_image_list_empty);
    BIND(clear_image_list);
    BIND(get_image_list_size);
    BIND(get_image_list_at);
    BIND(image_list_append);
    BIND(image_list_remove);
}
