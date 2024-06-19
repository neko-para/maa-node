#include "../include/helper.h"
#include "../include/info.h"
#include "../include/loader.h"

#include <MaaFramework/MaaAPI.h>

Napi::Value sync_context_run_task(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaSyncContextAPI>>().Data();
    auto task_name = info[1].As<Napi::String>().Utf8Value();
    auto param = info[2].As<Napi::String>().Utf8Value();
    auto worker = new SimpleAsyncWork<bool>(
        info.Env(),
        [=]() { return MaaSyncContextRunTask(handle, task_name.c_str(), param.c_str()); },
        [](auto env, auto res) { return Napi::Boolean::New(env, res); });
    worker->Queue();
    return worker->Promise();
}

Napi::Value sync_context_run_recognition(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaSyncContextAPI>>().Data();
    auto image = info[1].As<Napi::External<MaaImageBuffer>>().Data();
    auto task_name = info[2].As<Napi::String>().Utf8Value();
    auto task_param = info[3].As<Napi::String>().Utf8Value();

    using R = std::optional<std::tuple<MaaRect, std::string>>;

    auto worker = new SimpleAsyncWork<R>(
        info.Env(),
        [=]() -> R {
            MaaRect out_box;
            StringBuffer out_detail;
            auto ret = MaaSyncContextRunRecognition(
                handle,
                image,
                task_name.c_str(),
                task_param.c_str(),
                &out_box,
                out_detail);
            if (ret) {
                return std::tuple<MaaRect, std::string> { out_box, out_detail };
            }
            else {
                return std::nullopt;
            }
        },
        [](auto env, const R& res) -> Napi::Value {
            if (res.has_value()) {
                auto result = Napi::Object::New(env);
                result["out_box"] = FromRect(env, std::get<0>(res.value()));
                result["out_detail"] = std::get<1>(res.value());
                return result;
            }
            else {
                return env.Null();
            }
        });

    worker->Queue();
    return worker->Promise();
}

Napi::Value sync_context_run_action(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaSyncContextAPI>>().Data();
    auto task_name = info[1].As<Napi::String>().Utf8Value();
    auto task_param = info[2].As<Napi::String>().Utf8Value();
    auto cur_box = ToRect(info[3].As<Napi::Object>());
    auto cur_rec_detail = info[4].As<Napi::String>().Utf8Value();

    auto worker = new SimpleAsyncWork<bool>(
        info.Env(),
        [=]() {
            return MaaSyncContextRunAction(
                handle,
                task_name.c_str(),
                task_param.c_str(),
                const_cast<MaaRectHandle>(&cur_box),
                cur_rec_detail.c_str());
        },
        [](auto env, auto res) { return Napi::Boolean::New(env, res); });
    worker->Queue();
    return worker->Promise();
}

Napi::Value sync_context_click(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaSyncContextAPI>>().Data();
    auto x = info[1].As<Napi::Number>().Int32Value();
    auto y = info[2].As<Napi::Number>().Int32Value();

    auto worker = new SimpleAsyncWork<bool>(
        info.Env(),
        [=]() { return MaaSyncContextClick(handle, x, y); },
        [](auto env, auto res) { return Napi::Boolean::New(env, res); });
    worker->Queue();
    return worker->Promise();
}

Napi::Value sync_context_swipe(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaSyncContextAPI>>().Data();
    auto x1 = info[1].As<Napi::Number>().Int32Value();
    auto y1 = info[2].As<Napi::Number>().Int32Value();
    auto x2 = info[3].As<Napi::Number>().Int32Value();
    auto y2 = info[4].As<Napi::Number>().Int32Value();
    auto duration = info[5].As<Napi::Number>().Int32Value();

    auto worker = new SimpleAsyncWork<bool>(
        info.Env(),
        [=]() { return MaaSyncContextSwipe(handle, x1, y1, x2, y2, duration); },
        [](auto env, auto res) { return Napi::Boolean::New(env, res); });
    worker->Queue();
    return worker->Promise();
}

Napi::Value sync_context_press_key(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaSyncContextAPI>>().Data();
    auto key = info[1].As<Napi::Number>().Int32Value();

    auto worker = new SimpleAsyncWork<bool>(
        info.Env(),
        [=]() { return MaaSyncContextPressKey(handle, key); },
        [](auto env, auto res) { return Napi::Boolean::New(env, res); });
    worker->Queue();
    return worker->Promise();
}

Napi::Value sync_context_input_text(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaSyncContextAPI>>().Data();
    auto text = info[1].As<Napi::String>().Utf8Value();

    auto worker = new SimpleAsyncWork<bool>(
        info.Env(),
        [=]() { return MaaSyncContextInputText(handle, text.c_str()); },
        [](auto env, auto res) { return Napi::Boolean::New(env, res); });
    worker->Queue();
    return worker->Promise();
}

Napi::Value sync_context_touch_down(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaSyncContextAPI>>().Data();
    auto contact = info[1].As<Napi::Number>().Int32Value();
    auto x = info[2].As<Napi::Number>().Int32Value();
    auto y = info[3].As<Napi::Number>().Int32Value();
    auto pressure = info[4].As<Napi::Number>().Int32Value();

    auto worker = new SimpleAsyncWork<bool>(
        info.Env(),
        [=]() { return MaaSyncContextTouchDown(handle, contact, x, y, pressure); },
        [](auto env, auto res) { return Napi::Boolean::New(env, res); });
    worker->Queue();
    return worker->Promise();
}

Napi::Value sync_context_touch_move(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaSyncContextAPI>>().Data();
    auto contact = info[1].As<Napi::Number>().Int32Value();
    auto x = info[2].As<Napi::Number>().Int32Value();
    auto y = info[3].As<Napi::Number>().Int32Value();
    auto pressure = info[4].As<Napi::Number>().Int32Value();

    auto worker = new SimpleAsyncWork<bool>(
        info.Env(),
        [=]() { return MaaSyncContextTouchMove(handle, contact, x, y, pressure); },
        [](auto env, auto res) { return Napi::Boolean::New(env, res); });
    worker->Queue();
    return worker->Promise();
}

Napi::Value sync_context_touch_up(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaSyncContextAPI>>().Data();
    auto contact = info[1].As<Napi::Number>().Int32Value();

    auto worker = new SimpleAsyncWork<bool>(
        info.Env(),
        [=]() { return MaaSyncContextTouchUp(handle, contact); },
        [](auto env, auto res) { return Napi::Boolean::New(env, res); });
    worker->Queue();
    return worker->Promise();
}

Napi::Value sync_context_screencap(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaSyncContextAPI>>().Data();
    auto value = info[1].As<Napi::External<MaaImageBuffer>>().Data();

    auto worker = new SimpleAsyncWork<bool>(
        info.Env(),
        [=]() { return MaaSyncContextScreencap(handle, value); },
        [](auto env, auto res) { return Napi::Boolean::New(env, res); });
    worker->Queue();
    return worker->Promise();
}

Napi::Value sync_context_cached_image(const Napi::CallbackInfo& info)
{
    auto handle = info[0].As<Napi::External<MaaSyncContextAPI>>().Data();
    auto value = info[1].As<Napi::External<MaaImageBuffer>>().Data();

    auto worker = new SimpleAsyncWork<bool>(
        info.Env(),
        [=]() { return MaaSyncContextScreencap(handle, value); },
        [](auto env, auto res) { return Napi::Boolean::New(env, res); });
    worker->Queue();
    return worker->Promise();
}

void load_task_syncContext(Napi::Env env, Napi::Object& exports)
{
    BIND(sync_context_run_task);
    BIND(sync_context_run_recognition);
    BIND(sync_context_run_action);
    BIND(sync_context_click);
    BIND(sync_context_swipe);
    BIND(sync_context_press_key);
    BIND(sync_context_input_text);
    BIND(sync_context_touch_down);
    BIND(sync_context_touch_move);
    BIND(sync_context_touch_up);
    BIND(sync_context_screencap);
    BIND(sync_context_cached_image);
}
