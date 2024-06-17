#pragma once

#include <napi.h>

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
