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

    static Impl* FromValueOrNull(Napi::Value value)
    {
        if (value.IsNull()) {
            return nullptr;
        }
        else {
            return FromValue(value);
        }
    }

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
    bool disposed = false;

    void dispose()
    {
        if (disposed) {
            return;
        }
        disposed = true;
        MaaControllerDestroy(handle);
        if (callback) {
            delete callback;
        }
    }

    ~ControllerInfo() { dispose(); }
};

struct ResourceInfo : InfoBase<MaaResourceHandle, ResourceInfo>
{
    CallbackContext* callback = nullptr;
    bool disposed = false;

    void dispose()
    {
        if (disposed) {
            return;
        }
        disposed = true;
        MaaResourceDestroy(handle);
        if (callback) {
            delete callback;
        }
    }

    ~ResourceInfo() { dispose(); }
};

struct InstanceInfo : InfoBase<MaaInstanceHandle, InstanceInfo>
{
    CallbackContext* callback = nullptr;
    Napi::Reference<Napi::External<ResourceInfo>> resource;
    Napi::Reference<Napi::External<ControllerInfo>> controller;
    std::map<std::string, CallbackContext*> custom_recognizers;
    std::map<std::string, CallbackContext*> custom_actions;
    bool disposed = false;

    void dispose()
    {
        if (disposed) {
            return;
        }
        disposed = true;
        MaaDestroy(handle);
        if (callback) {
            delete callback;
        }
        ClearRecos();
        ClearActs();
    }

    ~InstanceInfo() { dispose(); }

    void ClearRecos()
    {
        for (const auto& [name, cb] : custom_recognizers) {
            delete cb;
        }
        custom_recognizers.clear();
    }

    void ClearActs()
    {
        for (const auto& [name, cb] : custom_actions) {
            delete cb;
        }
        custom_actions.clear();
    }
};
