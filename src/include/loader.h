#include <napi.h>

struct ExtContextInfo;

void load_instance_context(
    Napi::Env env,
    Napi::Object& exports,
    Napi::External<ExtContextInfo> context);
void load_instance_controller(
    Napi::Env env,
    Napi::Object& exports,
    Napi::External<ExtContextInfo> context);
void load_instance_resource(
    Napi::Env env,
    Napi::Object& exports,
    Napi::External<ExtContextInfo> context);
void load_instance_tasker(
    Napi::Env env,
    Napi::Object& exports,
    Napi::External<ExtContextInfo> context);

void load_utility_buffer(
    Napi::Env env,
    Napi::Object& exports,
    Napi::External<ExtContextInfo> context);
void load_utility_utility(
    Napi::Env env,
    Napi::Object& exports,
    Napi::External<ExtContextInfo> context);

void load_config_config(
    Napi::Env env,
    Napi::Object& exports,
    Napi::External<ExtContextInfo> context);

void load_device_device(
    Napi::Env env,
    Napi::Object& exports,
    Napi::External<ExtContextInfo> context);

void load_win32_win32Window(
    Napi::Env env,
    Napi::Object& exports,
    Napi::External<ExtContextInfo> context);
