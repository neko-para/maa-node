#pragma once

#include <MaaFramework/MaaAPI.h>
#include <napi.h>

#include <functional>
#include <future>

template <typename Result>
class SimpleAsyncWork : public Napi::AsyncWorker
{
public:
    SimpleAsyncWork(
        Napi::Env env,
        std::function<Result()> execute,
        std::function<Napi::Value(Napi::Env, const Result&)> ok)
        : AsyncWorker(env)
        , execute(execute)
        , ok(ok)
        , deferred(Napi::Promise::Deferred::New(env))
    {
    }

    void Execute() override { result = execute(); }

    void OnOK() override { deferred.Resolve(ok(Env(), result)); }

    void OnError(const Napi::Error& e) override { deferred.Reject(e.Value()); }

    Napi::Promise Promise() { return deferred.Promise(); }

private:
    Result result;
    std::function<Result()> execute;
    std::function<Napi::Value(Napi::Env, const Result&)> ok;
    Napi::Promise::Deferred deferred;
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

template <typename Type>
inline void DeleteFinalizer(Napi::Env env, Type data)
{
    delete data;
}

inline void NotificationCallback(const char* msg, const char* details, void* arg)
{
    auto ctx = reinterpret_cast<CallbackContext*>(arg);
    ctx->Call<void>(
        [=](auto env, auto fn) {
            return fn.Call({ Napi::String::New(env, msg), Napi::String::New(env, details) });
        },
        [](auto res) { std::ignore = res; });
}

struct StringBuffer
{
    MaaStringBuffer* buffer;
    bool own;

    StringBuffer()
        : buffer(MaaStringBufferCreate())
        , own(true)
    {
    }

    StringBuffer(MaaStringBuffer* handle)
        : buffer(handle)
        , own(false)
    {
    }

    ~StringBuffer()
    {
        if (own) {
            MaaStringBufferDestroy(buffer);
        }
    }

    operator MaaStringBuffer*() const { return buffer; }

    std::string str() const
    {
        return std::string(MaaStringBufferGet(buffer), MaaStringBufferSize(buffer));
    }

    void set(std::string_view data) const
    {
        MaaStringBufferSetEx(buffer, data.data(), data.size());
    }
};
