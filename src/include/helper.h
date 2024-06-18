#pragma once

#include <MaaFramework/MaaAPI.h>
#include <napi.h>

#include <future>
#include <optional>
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

    CallbackContext(Napi::Env env, Napi::Function cb, const char* name)
        : fn(cb)
        , tsfn(Napi::ThreadSafeFunction::New(env, fn, name, 0, 1))
    {
    }

    ~CallbackContext() { tsfn.Release(); }

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

inline void TrivialCallback(MaaStringView msg, MaaStringView details, MaaCallbackTransparentArg arg)
{
    auto ctx = reinterpret_cast<CallbackContext*>(arg);
    std::string msg_str = msg;
    std::string details_str = details;
    ctx->Call<void>(
        [msg_str, details_str](auto env, auto fn) {
            return fn.Call(
                { Napi::String::New(env, msg_str), Napi::String::New(env, details_str) });
        },
        [](auto res) { std::ignore = res; });
}

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

inline Napi::Object FromRect(const MaaRect& rect)
{
    Napi::Object rc;
    rc["x"] = rect.x;
    rc["y"] = rect.y;
    rc["width"] = rect.width;
    rc["height"] = rect.height;
    return rc;
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

#define BIND(name) exports[#name] = Napi::Function::New(env, name, #name)
