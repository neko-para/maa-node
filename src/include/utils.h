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

template <typename Handle, Handle* (*Create)(), void (*Destroy)(Handle*)>
struct HandlerHolder
{
    Handle* buffer;
    bool own;

    HandlerHolder()
        : buffer(Create())
        , own(true)
    {
    }

    HandlerHolder(Handle* handle, bool own)
        : buffer(handle)
        , own(own)
    {
    }

    HandlerHolder(const HandlerHolder& holder) = delete;

    HandlerHolder(HandlerHolder&& holder)
    {
        buffer = holder.buffer;
        own = holder.own;
        holder.own = false;
    }

    ~HandlerHolder()
    {
        if (own) {
            Destroy(buffer);
        }
    }

    HandlerHolder& operator=(const HandlerHolder& holder) = delete;

    HandlerHolder& operator=(HandlerHolder&& holder)
    {
        if (&holder == this) {
            return *this;
        }
        if (own) {
            Destroy(buffer);
        }
        buffer = holder.buffer;
        own = holder.own;
        holder.own = false;
        return *this;
    }
};

template <typename Handle>
struct HandlerReferHolder
{
    Handle* buffer;

    HandlerReferHolder(Handle* handle)
        : buffer(handle)
    {
    }

    HandlerReferHolder(const HandlerReferHolder& holder) = default;

    HandlerReferHolder& operator=(const HandlerReferHolder& holder) = delete;
};

struct StringBuffer
    : public HandlerHolder<MaaStringBuffer, &MaaStringBufferCreate, &MaaStringBufferDestroy>
{
    using HandlerHolder::HandlerHolder;

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

struct StringBufferRefer : public HandlerReferHolder<const MaaStringBuffer>
{
    using HandlerReferHolder::HandlerReferHolder;

    operator const MaaStringBuffer*() const { return buffer; }

    std::string str() const
    {
        return std::string(MaaStringBufferGet(buffer), MaaStringBufferSize(buffer));
    }
};

struct ImageBuffer
    : public HandlerHolder<MaaImageBuffer, &MaaImageBufferCreate, &MaaImageBufferDestroy>

{
    using HandlerHolder::HandlerHolder;

    operator MaaImageBuffer*() const { return buffer; }

    Napi::ArrayBuffer data(Napi::Env env) const
    {
        return Napi::ArrayBuffer::New(
            env,
            MaaImageBufferGetEncoded(buffer),
            MaaImageBufferGetEncodedSize(buffer));
    }

    void set(Napi::ArrayBuffer data) const
    {
        MaaImageBufferSetEncoded(
            buffer,
            reinterpret_cast<uint8_t*>(data.Data()),
            data.ByteLength());
    }
};

struct ImageBufferRefer : public HandlerReferHolder<const MaaImageBuffer>
{
    using HandlerReferHolder::HandlerReferHolder;

    operator const MaaImageBuffer*() const { return buffer; }

    Napi::ArrayBuffer data(Napi::Env env) const
    {
        return Napi::ArrayBuffer::New(
            env,
            MaaImageBufferGetEncoded(buffer),
            MaaImageBufferGetEncodedSize(buffer));
    }
};

template <
    typename ListHandle,
    typename Handle,
    typename Wrapper,
    ListHandle* (*Create)(),
    void (*Destroy)(ListHandle*),
    MaaBool (*IsEmpty)(const ListHandle*),
    MaaSize (*Size)(const ListHandle*),
    const Handle* (*At)(const ListHandle*, MaaSize),
    MaaBool (*Append)(ListHandle*, const Handle*),
    MaaBool (*Remove)(ListHandle*, MaaSize),
    MaaBool (*Clear)(ListHandle*)>
struct ListBuffer
{
    ListHandle* buffer;
    bool own;

    ListBuffer()
        : buffer(Create())
        , own(true)
    {
    }

    ListBuffer(ListHandle* handle)
        : buffer(handle)
        , own(false)
    {
    }

    ~ListBuffer()
    {
        if (own) {
            Destroy(buffer);
        }
    }

    operator ListHandle*() const { return buffer; }

    size_t size() const { return Size(buffer); }

    Wrapper at(size_t i) const { return Wrapper(At(buffer, i)); }

    bool append(const Wrapper& wrapper) { Append(buffer, wrapper); }

    bool remove(size_t i) { Remove(buffer, i); }

    void clear() { Clear(buffer); }

    template <typename Mapper>
    auto as_vector(Mapper func) const
    {
        std::vector<decltype(func(std::declval<Wrapper>()))> result;
        auto sz = size();
        result.reserve(sz);
        for (size_t i = 0; i < sz; i++) {
            result.push_back(func(at(i)));
        }
        return result;
    }
};

using StringListBuffer = ListBuffer<
    MaaStringListBuffer,
    MaaStringBuffer,
    StringBufferRefer,
    &MaaStringListBufferCreate,
    &MaaStringListBufferDestroy,
    &MaaStringListBufferIsEmpty,
    &MaaStringListBufferSize,
    &MaaStringListBufferAt,
    &MaaStringListBufferAppend,
    &MaaStringListBufferRemove,
    &MaaStringListBufferClear>;

using ImageListBuffer = ListBuffer<
    MaaImageListBuffer,
    MaaImageBuffer,
    ImageBufferRefer,
    &MaaImageListBufferCreate,
    &MaaImageListBufferDestroy,
    &MaaImageListBufferIsEmpty,
    &MaaImageListBufferSize,
    &MaaImageListBufferAt,
    &MaaImageListBufferAppend,
    &MaaImageListBufferRemove,
    &MaaImageListBufferClear>;

