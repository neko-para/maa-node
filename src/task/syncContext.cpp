#include "../include/helper.h"
#include "../include/info.h"
#include "../include/loader.h"

#include <MaaFramework/MaaAPI.h>

Napi::Value sync_context_run_task(const Napi::CallbackInfo& info)
{
    CheckCount(info, 3);
    auto handle = CheckAsExternal<MaaSyncContextAPI>(info[0]).Data();
    auto task_name = CheckAsString(info[1]);
    auto param = CheckAsString(info[2]);
    auto worker = new SimpleAsyncWork<bool>(
        info.Env(),
        [=]() { return MaaSyncContextRunTask(handle, task_name.c_str(), param.c_str()); },
        [](auto env, auto res) { return Napi::Boolean::New(env, res); });
    worker->Queue();
    return worker->Promise();
}

Napi::Value sync_context_run_recognition(const Napi::CallbackInfo& info)
{
    CheckCount(info, 4);
    auto handle = CheckAsExternal<MaaSyncContextAPI>(info[0]).Data();
    auto image = CheckAsExternal<MaaImageBuffer>(info[1]).Data();
    auto task_name = CheckAsString(info[2]);
    auto task_param = CheckAsString(info[3]);

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
                return std::tuple<MaaRect, std::string> { out_box, out_detail.str() };
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
    CheckCount(info, 5);
    auto handle = CheckAsExternal<MaaSyncContextAPI>(info[0]).Data();
    auto task_name = CheckAsString(info[1]);
    auto task_param = CheckAsString(info[2]);
    auto cur_box = ToRect(CheckAsObject(info[3]));
    auto cur_rec_detail = CheckAsString(info[4]);

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
    CheckCount(info, 3);
    auto handle = CheckAsExternal<MaaSyncContextAPI>(info[0]).Data();
    auto x = CheckAsNumber(info[1]).Int32Value();
    auto y = CheckAsNumber(info[2]).Int32Value();

    auto worker = new SimpleAsyncWork<bool>(
        info.Env(),
        [=]() { return MaaSyncContextClick(handle, x, y); },
        [](auto env, auto res) { return Napi::Boolean::New(env, res); });
    worker->Queue();
    return worker->Promise();
}

Napi::Value sync_context_swipe(const Napi::CallbackInfo& info)
{
    CheckCount(info, 6);
    auto handle = CheckAsExternal<MaaSyncContextAPI>(info[0]).Data();
    auto x1 = CheckAsNumber(info[1]).Int32Value();
    auto y1 = CheckAsNumber(info[2]).Int32Value();
    auto x2 = CheckAsNumber(info[3]).Int32Value();
    auto y2 = CheckAsNumber(info[4]).Int32Value();
    auto duration = CheckAsNumber(info[5]).Int32Value();

    auto worker = new SimpleAsyncWork<bool>(
        info.Env(),
        [=]() { return MaaSyncContextSwipe(handle, x1, y1, x2, y2, duration); },
        [](auto env, auto res) { return Napi::Boolean::New(env, res); });
    worker->Queue();
    return worker->Promise();
}

Napi::Value sync_context_press_key(const Napi::CallbackInfo& info)
{
    CheckCount(info, 2);
    auto handle = CheckAsExternal<MaaSyncContextAPI>(info[0]).Data();
    auto key = CheckAsNumber(info[1]).Int32Value();

    auto worker = new SimpleAsyncWork<bool>(
        info.Env(),
        [=]() { return MaaSyncContextPressKey(handle, key); },
        [](auto env, auto res) { return Napi::Boolean::New(env, res); });
    worker->Queue();
    return worker->Promise();
}

Napi::Value sync_context_input_text(const Napi::CallbackInfo& info)
{
    CheckCount(info, 2);
    auto handle = CheckAsExternal<MaaSyncContextAPI>(info[0]).Data();
    auto text = CheckAsString(info[1]);

    auto worker = new SimpleAsyncWork<bool>(
        info.Env(),
        [=]() { return MaaSyncContextInputText(handle, text.c_str()); },
        [](auto env, auto res) { return Napi::Boolean::New(env, res); });
    worker->Queue();
    return worker->Promise();
}

Napi::Value sync_context_touch_down(const Napi::CallbackInfo& info)
{
    CheckCount(info, 5);
    auto handle = CheckAsExternal<MaaSyncContextAPI>(info[0]).Data();
    auto contact = CheckAsNumber(info[1]).Int32Value();
    auto x = CheckAsNumber(info[2]).Int32Value();
    auto y = CheckAsNumber(info[3]).Int32Value();
    auto pressure = CheckAsNumber(info[4]).Int32Value();

    auto worker = new SimpleAsyncWork<bool>(
        info.Env(),
        [=]() { return MaaSyncContextTouchDown(handle, contact, x, y, pressure); },
        [](auto env, auto res) { return Napi::Boolean::New(env, res); });
    worker->Queue();
    return worker->Promise();
}

Napi::Value sync_context_touch_move(const Napi::CallbackInfo& info)
{
    CheckCount(info, 5);
    auto handle = CheckAsExternal<MaaSyncContextAPI>(info[0]).Data();
    auto contact = CheckAsNumber(info[1]).Int32Value();
    auto x = CheckAsNumber(info[2]).Int32Value();
    auto y = CheckAsNumber(info[3]).Int32Value();
    auto pressure = CheckAsNumber(info[4]).Int32Value();

    auto worker = new SimpleAsyncWork<bool>(
        info.Env(),
        [=]() { return MaaSyncContextTouchMove(handle, contact, x, y, pressure); },
        [](auto env, auto res) { return Napi::Boolean::New(env, res); });
    worker->Queue();
    return worker->Promise();
}

Napi::Value sync_context_touch_up(const Napi::CallbackInfo& info)
{
    CheckCount(info, 2);
    auto handle = CheckAsExternal<MaaSyncContextAPI>(info[0]).Data();
    auto contact = CheckAsNumber(info[1]).Int32Value();

    auto worker = new SimpleAsyncWork<bool>(
        info.Env(),
        [=]() { return MaaSyncContextTouchUp(handle, contact); },
        [](auto env, auto res) { return Napi::Boolean::New(env, res); });
    worker->Queue();
    return worker->Promise();
}

Napi::Value sync_context_screencap(const Napi::CallbackInfo& info)
{
    CheckCount(info, 2);
    auto handle = CheckAsExternal<MaaSyncContextAPI>(info[0]).Data();
    auto value = CheckAsExternal<MaaImageBuffer>(info[1]).Data();

    auto worker = new SimpleAsyncWork<bool>(
        info.Env(),
        [=]() { return MaaSyncContextScreencap(handle, value); },
        [](auto env, auto res) { return Napi::Boolean::New(env, res); });
    worker->Queue();
    return worker->Promise();
}

Napi::Value sync_context_cached_image(const Napi::CallbackInfo& info)
{
    CheckCount(info, 2);
    auto handle = CheckAsExternal<MaaSyncContextAPI>(info[0]).Data();
    auto value = CheckAsExternal<MaaImageBuffer>(info[1]).Data();

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
