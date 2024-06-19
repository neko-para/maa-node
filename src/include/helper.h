#pragma once

#include <MaaFramework/MaaAPI.h>
#include <napi.h>

#include <future>
#include <iostream>
#include <optional>
#include <string>
#include <tuple>
#include <type_traits>

template <typename Result>
class SimpleAsyncWork : public Napi::AsyncWorker
{
public:
    SimpleAsyncWork(
        const Napi::Function& cb,
        std::function<Result()> execute,
        std::function<Napi::Value(Napi::Env, const Result&)> ok)
        : AsyncWorker(cb)
        , execute(execute)
        , ok(ok)
    {
    }

    void Execute() override { result = execute(); }

    void OnOK() override
    {
        auto env = Env();
        Callback().Call({ env.Undefined(), ok(env, result) });
    }

    void OnError(const Napi::Error& e) override
    {
        auto env = Env();
        Callback().Call({ e.Value(), env.Undefined() });
    }

private:
    Result result;
    std::function<Result()> execute;
    std::function<Napi::Value(Napi::Env, const Result&)> ok;
};

struct CallbackContext
{
    Napi::Function fn;
    Napi::ThreadSafeFunction tsfn;
    const char* name;

    CallbackContext(Napi::Env env, Napi::Function cb, const char* name)
        : fn(cb)
        , tsfn(Napi::ThreadSafeFunction::New(env, fn, name, 0, 1))
        , name(name)
    {
    }

    ~CallbackContext()
    {
        std::cerr << "destroy " << name << std::endl;
        tsfn.Release();
    }

    template <typename Result>
    Result Call(
        std::function<Napi::Value(Napi::Env, Napi::Function)> caller,
        std::function<Result(Napi::Value)> parser)
    {
        std::promise<Result> promise;
        std::future<Result> future = promise.get_future();
        tsfn.BlockingCall([&caller, &parser, &promise](Napi::Env env, Napi::Function fn) {
            Napi::Value result = caller(env, fn);
            if (result.IsPromise()) {
                Napi::Object resultObject = result.As<Napi::Object>();
                resultObject.Get("then").As<Napi::Function>().Call(
                    resultObject,
                    {
                        Napi::Function::New(
                            env,
                            [&promise, &parser](const Napi::CallbackInfo& info) {
                                if constexpr (std::is_same_v<Result, void>) {
                                    parser(info[0]);
                                    promise.set_value();
                                }
                                else {
                                    promise.set_value(parser(info[0]));
                                }
                                return info.Env().Undefined();
                            }),
                        Napi::Function::New(
                            env,
                            [&promise, &parser](const Napi::CallbackInfo& info) {
                                if constexpr (std::is_same_v<Result, void>) {
                                    parser(info[0]);
                                    promise.set_value();
                                }
                                else {
                                    promise.set_value(parser(info[0]));
                                }
                                info.Env().Undefined();
                            }),
                    });
            }
            else {
                if constexpr (std::is_same_v<Result, void>) {
                    parser(result);
                    promise.set_value();
                }
                else {
                    promise.set_value(parser(result));
                }
            }
        });
        return future.get();
    }
};

inline Napi::Object FromRect(Napi::Env env, const MaaRect& rect)
{
    auto rc = Napi::Object::New(env);
    rc["x"] = rect.x;
    rc["y"] = rect.y;
    rc["width"] = rect.width;
    rc["height"] = rect.height;
    return rc;
}

inline MaaRect ToRect(Napi::Object rc)
{
    return MaaRect {
        Napi::Value(rc["x"]).As<Napi::Number>().Int32Value(),
        Napi::Value(rc["y"]).As<Napi::Number>().Int32Value(),
        Napi::Value(rc["width"]).As<Napi::Number>().Int32Value(),
        Napi::Value(rc["height"]).As<Napi::Number>().Int32Value(),
    };
}

inline void TrivialCallback(MaaStringView msg, MaaStringView details, MaaCallbackTransparentArg arg)
{
    auto ctx = reinterpret_cast<CallbackContext*>(arg);
    ctx->Call<void>(
        [=](auto env, auto fn) {
            return fn.Call({ Napi::String::New(env, msg), Napi::String::New(env, details) });
        },
        [](auto res) { std::ignore = res; });
}

inline MaaBool CustomRecognizerAnalyze(
    MaaSyncContextHandle sync_context,
    MaaImageBufferHandle image,
    MaaStringView task_name,
    MaaStringView custom_recognition_param,
    MaaTransparentArg arg,
    MaaRectHandle out_box,
    MaaStringBufferHandle out_detail)
{
    auto ctx = reinterpret_cast<CallbackContext*>(arg);
    using R = std::optional<std::tuple<MaaRect, std::string>>;
    auto res = ctx->Call<R>(
        [=](auto env, auto fn) {
            return fn.Call({ Napi::External<MaaSyncContextAPI>::New(env, sync_context),
                             Napi::External<MaaImageBuffer>::New(env, image),
                             Napi::String::New(env, task_name),
                             Napi::String::New(env, custom_recognition_param) });
        },
        [](Napi::Value res) -> R {
            if (res.IsNull()) {
                return std::nullopt;
            }
            else {
                auto obj = res.As<Napi::Object>();
                return std::tuple<MaaRect, std::string> {
                    ToRect(Napi::Value(obj["out_box"]).As<Napi::Object>()),
                    Napi::Value(obj["out_detail"]).As<Napi::String>().Utf8Value()
                };
            }
        });

    if (res.has_value()) {
        *out_box = std::get<0>(res.value());
        auto& str = std::get<1>(res.value());
        MaaSetStringEx(out_detail, str.c_str(), str.size());
        return true;
    }
    else {
        return false;
    }
}

inline MaaBool CustomActionRun(
    MaaSyncContextHandle sync_context,
    MaaStringView task_name,
    MaaStringView custom_action_param,
    MaaRectHandle cur_box,
    MaaStringView cur_rec_detail,
    MaaTransparentArg arg)
{
    auto ctx = reinterpret_cast<CallbackContext*>(arg);

    auto res = ctx->Call<bool>(
        [=](auto env, auto fn) {
            return fn.Call({
                Napi::External<MaaSyncContextAPI>::New(env, sync_context),
                Napi::String::New(env, task_name),
                Napi::String::New(env, custom_action_param),
                FromRect(env, *cur_box),
                Napi::String::New(env, cur_rec_detail),
            });
        },
        [](Napi::Value res) { return res.As<Napi::Boolean>().Value(); });

    return res;
}

inline MaaCustomRecognizerAPI custom_recognizer_api = { CustomRecognizerAnalyze };
inline MaaCustomActionAPI custom_action_api = { CustomActionRun, nullptr };

template <typename Type>
inline std::optional<Type*> ExternalOrNull(Napi::Value value)
{
    if (value.IsNull()) {
        return std::nullopt;
    }
    else {
        return value.As<Napi::External<Type>>().Data();
    }
}

struct StringBuffer
{
    MaaStringBufferHandle buffer;

    StringBuffer()
        : buffer(MaaCreateStringBuffer())
    {
    }

    ~StringBuffer() { MaaDestroyStringBuffer(buffer); }

    operator MaaStringBufferHandle() const { return buffer; }

    operator std::string() const
    {
        return std::string(MaaGetString(buffer), MaaGetStringSize(buffer));
    }

    void set(std::string_view data) const { MaaSetStringEx(buffer, data.data(), data.size()); }
};

template <typename Type>
inline void DeleteFinalizer(Napi::Env env, Type data)
{
    delete data;
}

#define BIND(name) exports[#name] = Napi::Function::New(env, name, #name)
