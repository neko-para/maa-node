#pragma once

#include <MaaFramework/MaaAPI.h>
#include <exception>
#include <napi.h>

#include <future>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>

#define FMT_HEADER_ONLY
#include "fmt/format.h"

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

struct StringBuffer
{
    MaaStringBufferHandle buffer;
    bool own;

    StringBuffer()
        : buffer(MaaCreateStringBuffer())
        , own(true)
    {
    }

    StringBuffer(MaaStringBufferHandle handle)
        : buffer(handle)
        , own(false)
    {
    }

    ~StringBuffer()
    {
        if (own) {
            MaaDestroyStringBuffer(buffer);
        }
    }

    operator MaaStringBufferHandle() const { return buffer; }

    operator std::string() const
    {
        return std::string(MaaGetString(buffer), MaaGetStringSize(buffer));
    }

    void set(std::string_view data) const { MaaSetStringEx(buffer, data.data(), data.size()); }
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

inline std::tuple<int32_t, int32_t> ToSize(Napi::Object rc)
{
    return std::make_tuple(
        Napi::Value(rc["width"]).As<Napi::Number>().Int32Value(),
        Napi::Value(rc["height"]).As<Napi::Number>().Int32Value());
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
                return R::value_type {
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

inline MaaBool CustomControllerConnect(MaaTransparentArg handle_arg)
{
    auto ctx = reinterpret_cast<CallbackContext*>(handle_arg);

    auto res = ctx->Call<bool>(
        [](auto env, auto fn) { return fn.Call({ Napi::String::New(env, "connect") }); },
        [](Napi::Value res) { return res.As<Napi::Boolean>().Value(); });

    return res;
}

inline MaaBool
    CustomControllerRequestUUID(MaaTransparentArg handle_arg, MaaStringBufferHandle buffer)
{
    auto ctx = reinterpret_cast<CallbackContext*>(handle_arg);
    using R = std::optional<std::string>;
    auto res = ctx->Call<R>(
        [](auto env, auto fn) { return fn.Call({ Napi::String::New(env, "request_uuid") }); },
        [=](Napi::Value res) -> R {
            if (res.IsNull()) {
                return std::nullopt;
            }
            else {
                auto obj = res.As<Napi::String>();
                return R::value_type { obj.Utf8Value() };
            }
        });

    if (res.has_value()) {
        StringBuffer buf(buffer);
        buf.set(res.value());
        return true;
    }
    else {
        return false;
    }
}

inline MaaBool
    CustomControllerRequestResolution(MaaTransparentArg handle_arg, int32_t* width, int32_t* height)
{
    auto ctx = reinterpret_cast<CallbackContext*>(handle_arg);
    using R = std::optional<std::tuple<int32_t, int32_t>>;
    auto res = ctx->Call<R>(
        [](auto env, auto fn) { return fn.Call({ Napi::String::New(env, "request_uuid") }); },
        [=](Napi::Value res) -> R {
            if (res.IsNull()) {
                return std::nullopt;
            }
            else {
                auto obj = res.As<Napi::Object>();
                return R::value_type {
                    ToSize(obj),
                };
            }
        });

    if (res.has_value()) {
        *width = std::get<0>(res.value());
        *height = std::get<1>(res.value());
        return true;
    }
    else {
        return false;
    }
}

inline MaaBool CustomControllerStartApp(MaaStringView intent, MaaTransparentArg handle_arg)
{
    auto ctx = reinterpret_cast<CallbackContext*>(handle_arg);

    auto res = ctx->Call<bool>(
        [=](auto env, auto fn) {
            return fn.Call({ Napi::String::New(env, "start_app"), Napi::String::New(env, intent) });
        },
        [](Napi::Value res) { return res.As<Napi::Boolean>().Value(); });

    return res;
}

inline MaaBool CustomControllerStopApp(MaaStringView intent, MaaTransparentArg handle_arg)
{
    auto ctx = reinterpret_cast<CallbackContext*>(handle_arg);

    auto res = ctx->Call<bool>(
        [=](auto env, auto fn) {
            return fn.Call({ Napi::String::New(env, "stop_app"), Napi::String::New(env, intent) });
        },
        [](Napi::Value res) { return res.As<Napi::Boolean>().Value(); });

    return res;
}

inline MaaBool CustomControllerScreencap(MaaTransparentArg handle_arg, MaaImageBufferHandle buffer)
{
    auto ctx = reinterpret_cast<CallbackContext*>(handle_arg);

    auto res = ctx->Call<bool>(
        [=](auto env, auto fn) {
            return fn.Call({ Napi::String::New(env, "screencap"),
                             Napi::External<MaaImageBuffer>::New(env, buffer) });
        },
        [](Napi::Value res) { return res.As<Napi::Boolean>().Value(); });

    return res;
}

inline MaaBool CustomControllerClick(int32_t x, int32_t y, MaaTransparentArg handle_arg)
{
    auto ctx = reinterpret_cast<CallbackContext*>(handle_arg);

    auto res = ctx->Call<bool>(
        [=](auto env, auto fn) {
            return fn.Call({ Napi::String::New(env, "click"),
                             Napi::Number::New(env, x),
                             Napi::Number::New(env, y) });
        },
        [](Napi::Value res) { return res.As<Napi::Boolean>().Value(); });

    return res;
}

inline MaaBool CustomControllerSwipe(
    int32_t x1,
    int32_t y1,
    int32_t x2,
    int32_t y2,
    int32_t duration,
    MaaTransparentArg handle_arg)
{
    auto ctx = reinterpret_cast<CallbackContext*>(handle_arg);

    auto res = ctx->Call<bool>(
        [=](auto env, auto fn) {
            return fn.Call({ Napi::String::New(env, "swipe"),
                             Napi::Number::New(env, x1),
                             Napi::Number::New(env, y1),
                             Napi::Number::New(env, x2),
                             Napi::Number::New(env, y2),
                             Napi::Number::New(env, duration) });
        },
        [](Napi::Value res) { return res.As<Napi::Boolean>().Value(); });

    return res;
}

inline MaaBool CustomControllerTouchDown(
    int32_t contact,
    int32_t x,
    int32_t y,
    int32_t pressure,
    MaaTransparentArg handle_arg)
{
    auto ctx = reinterpret_cast<CallbackContext*>(handle_arg);

    auto res = ctx->Call<bool>(
        [=](auto env, auto fn) {
            return fn.Call({ Napi::String::New(env, "touch_down"),
                             Napi::Number::New(env, contact),
                             Napi::Number::New(env, y),
                             Napi::Number::New(env, x),
                             Napi::Number::New(env, pressure) });
        },
        [](Napi::Value res) { return res.As<Napi::Boolean>().Value(); });

    return res;
}

inline MaaBool CustomControllerTouchMove(
    int32_t contact,
    int32_t x,
    int32_t y,
    int32_t pressure,
    MaaTransparentArg handle_arg)
{
    auto ctx = reinterpret_cast<CallbackContext*>(handle_arg);

    auto res = ctx->Call<bool>(
        [=](auto env, auto fn) {
            return fn.Call({ Napi::String::New(env, "touch_move"),
                             Napi::Number::New(env, contact),
                             Napi::Number::New(env, y),
                             Napi::Number::New(env, x),
                             Napi::Number::New(env, pressure) });
        },
        [](Napi::Value res) { return res.As<Napi::Boolean>().Value(); });

    return res;
}

inline MaaBool CustomControllerTouchUp(int32_t contact, MaaTransparentArg handle_arg)
{
    auto ctx = reinterpret_cast<CallbackContext*>(handle_arg);

    auto res = ctx->Call<bool>(
        [=](auto env, auto fn) {
            return fn.Call({ Napi::String::New(env, "touch_up"), Napi::Number::New(env, contact) });
        },
        [](Napi::Value res) { return res.As<Napi::Boolean>().Value(); });

    return res;
}

inline MaaBool CustomControllerPressKey(int32_t keycode, MaaTransparentArg handle_arg)
{
    auto ctx = reinterpret_cast<CallbackContext*>(handle_arg);

    auto res = ctx->Call<bool>(
        [=](auto env, auto fn) {
            return fn.Call(
                { Napi::String::New(env, "press_key"), Napi::Number::New(env, keycode) });
        },
        [](Napi::Value res) { return res.As<Napi::Boolean>().Value(); });

    return res;
}

inline MaaBool CustomControllerInputText(MaaStringView text, MaaTransparentArg handle_arg)
{
    auto ctx = reinterpret_cast<CallbackContext*>(handle_arg);

    auto res = ctx->Call<bool>(
        [=](auto env, auto fn) {
            return fn.Call({ Napi::String::New(env, "input_text"), Napi::String::New(env, text) });
        },
        [](Napi::Value res) { return res.As<Napi::Boolean>().Value(); });

    return res;
}

inline MaaCustomRecognizerAPI custom_recognizer_api = { CustomRecognizerAnalyze };
inline MaaCustomActionAPI custom_action_api = { CustomActionRun, nullptr };
inline MaaCustomControllerAPI custom_controller_api = {
    CustomControllerConnect,   CustomControllerRequestUUID, CustomControllerRequestResolution,
    CustomControllerStartApp,  CustomControllerStopApp,     CustomControllerScreencap,
    CustomControllerClick,     CustomControllerSwipe,       CustomControllerTouchDown,
    CustomControllerTouchMove, CustomControllerTouchUp,     CustomControllerPressKey,
    CustomControllerInputText
};

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

template <typename Type>
inline void DeleteFinalizer(Napi::Env env, Type data)
{
    delete data;
}

struct MaaNodeException : public std::exception
{
    std::string error;

    MaaNodeException(std::string&& err)
        : error(err)
    {
    }

    virtual const char* what() const noexcept override { return error.c_str(); }
};

inline void CheckCount(const Napi::CallbackInfo& info, size_t count)
{
    if (info.Length() != count) {
        throw MaaNodeException { fmt::format("expect {} arguments, got {}", count, info.Length()) };
    }
}

inline std::string_view TypeOf(const Napi::Value& val)
{
    switch (val.Type()) {
    case napi_undefined:
        return "undefined";
    case napi_null:
        return "null";
    case napi_boolean:
        return "boolean";
    case napi_number:
        return "number";
    case napi_string:
        return "string";
    case napi_symbol:
        return "symbol";
    case napi_object:
        return "object";
    case napi_function:
        return "function";
    case napi_external:
        return "external";
    case napi_bigint:
        return "bigint";
    }
    return "unknown";
}

inline std::string CheckAsString(const Napi::Value& val)
{
    if (!val.IsString()) {
        throw MaaNodeException {
            fmt::format("expect string, got {} [{}]", val.ToString().Utf8Value(), TypeOf(val))
        };
    }
    return val.As<Napi::String>().Utf8Value();
}

inline Napi::Number CheckAsNumber(const Napi::Value& val)
{
    if (!val.IsNumber()) {
        throw MaaNodeException {
            fmt::format("expect number, got {} [{}]", val.ToString().Utf8Value(), TypeOf(val))
        };
    }
    return val.As<Napi::Number>();
}

inline Napi::Boolean CheckAsBoolean(const Napi::Value& val)
{
    if (!val.IsBoolean()) {
        throw MaaNodeException {
            fmt::format("expect boolean, got {} [{}]", val.ToString().Utf8Value(), TypeOf(val))
        };
    }
    return val.As<Napi::Boolean>();
}

inline Napi::Function CheckAsFunction(const Napi::Value& val)
{
    if (!val.IsFunction()) {
        throw MaaNodeException {
            fmt::format("expect function, got {} [{}]", val.ToString().Utf8Value(), TypeOf(val))
        };
    }
    return val.As<Napi::Function>();
}

inline Napi::Array CheckAsArray(const Napi::Value& val)
{
    if (!val.IsArray()) {
        throw MaaNodeException {
            fmt::format("expect array, got {} [{}]", val.ToString().Utf8Value(), TypeOf(val))
        };
    }
    return val.As<Napi::Array>();
}

inline Napi::Object CheckAsObject(const Napi::Value& val)
{
    if (!val.IsObject()) {
        throw MaaNodeException {
            fmt::format("expect object, got {} [{}]", val.ToString().Utf8Value(), TypeOf(val))
        };
    }
    return val.As<Napi::Object>();
}

template <typename T>
inline Napi::External<T> CheckAsExternal(const Napi::Value& val)
{
    if (!val.IsExternal()) {
        throw MaaNodeException {
            fmt::format("expect external, got {} [{}]", val.ToString().Utf8Value(), TypeOf(val))
        };
    }
    return val.As<Napi::External<T>>();
}

#define BIND(name)                                                                             \
    exports[#name] = Napi::Function::New(                                                      \
        env,                                                                                   \
        +[](const Napi::CallbackInfo& info) -> Napi::Value {                                   \
            try {                                                                              \
                return name(info);                                                             \
            }                                                                                  \
            catch (MaaNodeException exc) {                                                     \
                Napi::TypeError::New(info.Env(), fmt::format("maa.{}: {}", #name, exc.what())) \
                    .ThrowAsJavaScriptException();                                             \
                return info.Env().Null();                                                      \
            }                                                                                  \
        },                                                                                     \
        #name)
