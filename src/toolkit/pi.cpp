#include "../include/cb.h"
#include "../include/loader.h"
#include "../include/wrapper.h"

#include <MaaFramework/MaaAPI.h>
#include <MaaToolkit/MaaToolkitAPI.h>

Napi::Promise pi_run_cli(
    Napi::Env env,
    std::string resource_path,
    std::string user_path,
    bool directly,
    std::optional<Napi::Function> callback)
{
    MaaNotificationCallback cb = nullptr;
    CallbackContext* ctx = nullptr;

    if (callback) {
        cb = NotificationCallback;
        ctx = new CallbackContext { env, callback.value(), "NotificationCallback" };
    }

    auto worker = new SimpleAsyncWork<bool, "pi_run_cli">(
        env,
        [resource_path, user_path, directly, cb, ctx]() {
            return MaaToolkitProjectInterfaceRunCli(
                resource_path.c_str(),
                user_path.c_str(),
                directly,
                cb,
                ctx);
        },
        [ctx](auto env, auto res) {
            delete ctx;
            return JSConvert<bool>::to_value(env, res);
        });
    worker->Queue();
    return worker->Promise();
}

void load_toolkit_pi(Napi::Env env, Napi::Object& exports, Napi::External<ExtContextInfo> context)
{
    BIND(pi_run_cli);
}
