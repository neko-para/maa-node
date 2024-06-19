
#include <napi.h>

void load_instance_controller(Napi::Env env, Napi::Object& exports);
void load_instance_instance(Napi::Env env, Napi::Object& exports);
void load_instance_resource(Napi::Env env, Napi::Object& exports);

void load_utility_buffer(Napi::Env env, Napi::Object& exports);
void load_utility_utility(Napi::Env env, Napi::Object& exports);

void load_device_device(Napi::Env env, Napi::Object& exports);

void load_win32_window(Napi::Env env, Napi::Object& exports);
