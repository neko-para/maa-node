#pragma once

#include "./helper.h"

#include <MaaFramework/MaaAPI.h>

#include <iostream>
#include <map>
#include <string>

template <typename Type, typename Impl>
struct InfoBase
{
    Type handle = nullptr;

    static Impl* FromValue(Napi::Value value) { return value.As<Napi::External<Impl>>().Data(); }

    static Type HandleFromValue(Napi::Value value) { return FromValue(value)->handle; }

    static Type HandleFromValueOrNull(Napi::Value value)
    {
        if (value.IsNull()) {
            return nullptr;
        }
        else {
            return HandleFromValue(value);
        }
    }
};

struct ControllerInfo : InfoBase<MaaControllerHandle, ControllerInfo>
{
    CallbackContext* callback = nullptr;

    ~ControllerInfo()
    {
        std::cerr << "destroy controller" << std::endl;
        MaaControllerDestroy(handle);
        if (callback) {
            delete callback;
        }
    }
};

struct ResourceInfo : InfoBase<MaaResourceHandle, ResourceInfo>
{
    CallbackContext* callback = nullptr;

    ~ResourceInfo()
    {
        std::cerr << "destroy resource" << std::endl;
        MaaResourceDestroy(handle);
        if (callback) {
            delete callback;
        }
    }
};

struct InstanceInfo : InfoBase<MaaInstanceHandle, InstanceInfo>
{
    CallbackContext* callback = nullptr;
    std::map<std::string, CallbackContext*> custom_actions;

    ~InstanceInfo()
    {
        std::cerr << "destroy instance" << std::endl;
        MaaDestroy(handle);
        if (callback) {
            delete callback;
        }
        for (const auto& [name, cb] : custom_actions) {
            delete cb;
        }
    }
};
