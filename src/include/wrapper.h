#pragma once

#include <napi.h>   // IWYU pragma: export

#include <cstdint>  // IWYU pragma: export
#include <optional> // IWYU pragma: export
#include <vector>   // IWYU pragma: export

#include <exception>
#include <tuple>

#define FMT_HEADER_ONLY
#include "fmt/format.h"

#include <MaaFramework/MaaDef.h>

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

template <typename Type>
struct JSConvert
{
    static Type from_value(Napi::Value val) = delete;
    static Napi::Value to_value(Napi::Env env, const Type& val) = delete;
};

// msvc want this
template <>
struct JSConvert<void>
{
    static Napi::Value to_value(Napi::Env env) { return env.Undefined(); }
};

template <typename Type>
struct JSConvert<Napi::External<Type>>
{
    static Napi::External<Type> from_value(Napi::Value val)
    {
        return val.As<Napi::External<Type>>();
    }

    static Napi::Value to_value([[maybe_unused]] Napi::Env env, const Napi::External<Type>& val)
    {
        return val;
    }
};

template <>
struct JSConvert<Napi::Function>
{
    static Napi::Function from_value(Napi::Value val) { return val.As<Napi::Function>(); }

    static Napi::Value to_value([[maybe_unused]] Napi::Env env, const Napi::Function& val)
    {
        return val;
    }
};

template <>
struct JSConvert<Napi::Promise>
{
    static Napi::Promise from_value(Napi::Value val) { return val.As<Napi::Promise>(); }

    static Napi::Value to_value([[maybe_unused]] Napi::Env env, const Napi::Promise& val)
    {
        return val;
    }
};

template <>
struct JSConvert<Napi::ArrayBuffer>
{
    static Napi::ArrayBuffer from_value(Napi::Value val) { return val.As<Napi::ArrayBuffer>(); }

    static Napi::Value to_value([[maybe_unused]] Napi::Env env, const Napi::ArrayBuffer& val)
    {
        return val;
    }
};

template <>
struct JSConvert<std::string>
{
    static std::string from_value(Napi::Value val)
    {
        if (!val.IsString()) {
            throw MaaNodeException {
                fmt::format("expect string, got {} [{}]", val.ToString().Utf8Value(), TypeOf(val))
            };
        }
        return val.As<Napi::String>().Utf8Value();
    }

    static Napi::Value to_value(Napi::Env env, const std::string& val)
    {
        return Napi::String::New(env, val);
    }
};

template <>
struct JSConvert<bool>
{
    static bool from_value(Napi::Value val)
    {
        if (!val.IsBoolean()) {
            throw MaaNodeException {
                fmt::format("expect boolean, got {} [{}]", val.ToString().Utf8Value(), TypeOf(val))
            };
        }
        return val.As<Napi::Boolean>().Value();
    }

    static Napi::Value to_value(Napi::Env env, const bool& val)
    {
        return Napi::Boolean::New(env, val);
    }
};

template <>
struct JSConvert<int32_t>
{
    static int32_t from_value(Napi::Value val)
    {
        if (!val.IsNumber()) {
            throw MaaNodeException { fmt::format(
                "expect number(int32), got {} [{}]",
                val.ToString().Utf8Value(),
                TypeOf(val)) };
        }
        return val.As<Napi::Number>().Int32Value();
    }

    static Napi::Value to_value(Napi::Env env, const int32_t& val)
    {
        return Napi::Number::New(env, val);
    }
};

template <>
struct JSConvert<uint32_t>
{
    static uint32_t from_value(Napi::Value val)
    {
        if (!val.IsNumber()) {
            throw MaaNodeException { fmt::format(
                "expect number(uint32), got {} [{}]",
                val.ToString().Utf8Value(),
                TypeOf(val)) };
        }
        return val.As<Napi::Number>().Uint32Value();
    }

    static Napi::Value to_value(Napi::Env env, const uint32_t& val)
    {
        return Napi::Number::New(env, val);
    }
};

template <>
struct JSConvert<int64_t>
{
    static int64_t from_value(Napi::Value val)
    {
        if (!val.IsNumber() && !val.IsBigInt()) {
            throw MaaNodeException { fmt::format(
                "expect number(int64) or bigint, got {} [{}]",
                val.ToString().Utf8Value(),
                TypeOf(val)) };
        }
        if (val.IsNumber()) {
            return val.As<Napi::Number>().Int64Value();
        }
        else if (val.IsBigInt()) {
            return val.As<Napi::BigInt>().Int64Value(nullptr);
        }
        else {
            return 0;
        }
    }

    static Napi::Value to_value(Napi::Env env, const int64_t& val)
    {
        constexpr int64_t lower = -((1ll << 53) - 1);
        constexpr int64_t upper = (1ll << 53) - 1;
        if (lower <= val && val <= upper) {
            return Napi::Number::New(env, val);
        }
        else {
            return Napi::BigInt::New(env, val);
        }
    }
};

template <>
struct JSConvert<uint64_t>
{
    static uint64_t from_value(Napi::Value val)
    {
        if (!val.IsNumber() && !val.IsBigInt()) {
            throw MaaNodeException { fmt::format(
                "expect number(uint64) or bigint, got {} [{}]",
                val.ToString().Utf8Value(),
                TypeOf(val)) };
        }
        if (val.IsNumber()) {
            // Because of the restriction of safe integer, using int64_t is enough
            int64_t iv = val.As<Napi::Number>().Int64Value();
            return iv >= 0 ? static_cast<uint64_t>(iv) : 0;
        }
        else if (val.IsBigInt()) {
            return val.As<Napi::BigInt>().Uint64Value(nullptr);
        }
        else {
            return 0;
        }
    }

    static Napi::Value to_value(Napi::Env env, const uint64_t& val)
    {
        constexpr uint64_t upper = (1ull << 53) - 1;
        if (0 <= val && val <= upper) {
            return Napi::Number::New(env, val);
        }
        else {
            return Napi::BigInt::New(env, val);
        }
    }
};

template <typename T>
struct JSConvert<std::optional<T>>
{
    static std::optional<T> from_value(Napi::Value val)
    {
        if (val.IsNull()) {
            return std::nullopt;
        }
        else {
            return JSConvert<T>::from_value(val);
        }
    }

    static Napi::Value to_value(Napi::Env env, const std::optional<T>& val)
    {
        if (val.has_value()) {
            return JSConvert<T>::to_value(env, val.value());
        }
        else {
            return env.Null();
        }
    }
};

template <typename T>
struct JSConvert<std::vector<T>>
{
    static std::vector<T> from_value(Napi::Value val)
    {
        if (!val.IsArray()) {
            throw MaaNodeException { fmt::format(
                "expect array({}), got {} [{}]",
                typeid(T).name(),
                val.ToString().Utf8Value(),
                TypeOf(val)) };
        }
        auto arr = val.As<Napi::Array>();
        std::vector<T> res;
        res.reserve(arr.Length());
        for (auto [prop, subv] : arr) {
            res.push_back(JSConvert<T>::from_value(subv));
        }
        return res;
    }

    static Napi::Value to_value(Napi::Env env, const std::vector<T>& val)
    {
        auto arr = Napi::Array::New(env, val.size());
        for (size_t i = 0; i < val.size(); i++) {
            arr.Set(static_cast<uint32_t>(i), JSConvert<T>::to_value(env, val[i]));
        }
        return arr;
    }
};

template <typename... Args>
struct JSConvert<std::tuple<Args...>>
{
    using T = std::tuple<Args...>;

    static T from_value(Napi::Value val)
    {
        if (!val.IsArray()) {
            throw MaaNodeException { fmt::format(
                "expect array({}), got {} [{}]",
                typeid(T).name(),
                val.ToString().Utf8Value(),
                TypeOf(val)) };
        }
        auto arr = val.As<Napi::Array>();
        if (arr.Length() != std::tuple_size_v<T>) {
            throw MaaNodeException { fmt::format(
                "expect array of length {}, got {} [{}]",
                std::tuple_size_v<T>,
                val.ToString().Utf8Value(),
                TypeOf(val)) };
        }
        T result;
        [&]<std::size_t... I>(std::index_sequence<I...>) {
            ((std::get<I>(result) = JSConvert<std::tuple_element_t<I, T>>::from_value(arr[I])),
             ...);
        }(std::make_index_sequence<std::tuple_size_v<T>> {});
        return result;
    }

    static Napi::Value to_value(Napi::Env env, const T& val)
    {
        auto arr = Napi::Array::New(env, std::tuple_size_v<T>);
        [&]<std::size_t... I>(std::index_sequence<I...>) {
            ((arr.Set(I, JSConvert<std::tuple_element_t<I, T>>::to_value(env, std::get<I>(val)))),
             ...);
        }(std::make_index_sequence<std::tuple_size_v<T>> {});
        return arr;
    }
};

template <>
struct JSConvert<MaaRect>
{
    static MaaRect from_value(Napi::Value val)
    {
        if (!val.IsObject()) {
            throw MaaNodeException { fmt::format(
                "expect object(MaaRect), got {} [{}]",
                val.ToString().Utf8Value(),
                TypeOf(val)) };
        }
        auto obj = val.As<Napi::Object>();
        return MaaRect {
            JSConvert<int32_t>::from_value(obj.Get("x")),
            JSConvert<int32_t>::from_value(obj.Get("y")),
            JSConvert<int32_t>::from_value(obj.Get("width")),
            JSConvert<int32_t>::from_value(obj.Get("height")),
        };
    }

    static Napi::Value to_value(Napi::Env env, const MaaRect& val)
    {
        auto obj = Napi::Object::New(env);
        obj["x"] = val.x;
        obj["y"] = val.y;
        obj["width"] = val.width;
        obj["height"] = val.height;
        return obj;
    }
};

template <typename F>
struct FuncTraits
{
};

template <typename R, typename... Args>
struct FuncTraits<R (*)(Args...)>
{
    using ret = R;
    using args = std::tuple<Args...>;
    using call_args = args;
    static constexpr bool want_env = false;
};

template <typename R, typename... Args>
struct FuncTraits<R (*)(Napi::Env, Args...)>
{
    using ret = R;
    using args = std::tuple<Args...>;
    using call_args = std::tuple<Napi::Env, Args...>;
    static constexpr bool want_env = true;
};

template <size_t N>
struct StringHolder
{
    char data[N];

    constexpr StringHolder(const char (&str)[N]) { std::copy(str, str + N, data); }
};

template <typename Func, Func func, StringHolder name>
struct JSWrapFunction
{
    static auto make()
    {
        return +[](const Napi::CallbackInfo& info) -> Napi::Value {
            try {
                using Args = typename FuncTraits<Func>::args;
                using Ret = typename FuncTraits<Func>::ret;

                constexpr auto count = std::tuple_size_v<Args>;

                CheckCount(info, count);

                Args params;

                if constexpr (count > 0) {
                    [&]<std::size_t... I>(std::index_sequence<I...>) {
                        ((std::get<I>(params) =
                              JSConvert<std::tuple_element_t<I, Args>>::from_value(info[I])),
                         ...);
                    }(std::make_index_sequence<count> {});
                }

                Napi::Value ret;

                typename FuncTraits<Func>::call_args call_params = [p = std::move(params),
                                                                    &info]() {
                    if constexpr (FuncTraits<Func>::want_env) {
                        return std::tuple_cat(std::make_tuple(info.Env()), std::move(p));
                    }
                    else {
                        return p;
                    }
                }();

                if constexpr (std::is_same_v<Ret, void>) {
                    std::apply(func, std::move(call_params));
                    ret = info.Env().Undefined();
                }
                else {
                    ret = JSConvert<Ret>::to_value(
                        info.Env(),
                        std::apply(func, std::move(call_params)));
                }

                return ret;
            }
            catch (MaaNodeException exc) {
                Napi::TypeError::New(info.Env(), fmt::format("maa.{}: {}", name.data, exc.what()))
                    .ThrowAsJavaScriptException();
                return info.Env().Null();
            }
        };
    }
};

#define BIND(name)   \
    exports[#name] = \
        Napi::Function::New(env, JSWrapFunction<decltype(&name), &name, #name>::make(), #name)