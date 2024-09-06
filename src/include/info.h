#pragma once

#include "utils.h"
#include "wrapper.h"

#include <MaaFramework/MaaAPI.h>

template <typename Type, typename Impl>
struct InfoBase
{
    Type handle = nullptr;
};

struct ControllerInfo : InfoBase<MaaController*, ControllerInfo>
{
    CallbackContext* callback = nullptr;
    CallbackContext* custom_controller = nullptr;
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
        if (custom_controller) {
            delete custom_controller;
        }
    }

    ~ControllerInfo() { dispose(); }
};

struct ResourceInfo : InfoBase<MaaResource*, ResourceInfo>
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

struct TaskerInfo : InfoBase<MaaTasker*, TaskerInfo>
{
    CallbackContext* callback = nullptr;
    Napi::Reference<Napi::External<ResourceInfo>> resource;
    Napi::Reference<Napi::External<ControllerInfo>> controller;
    // std::map<std::string, CallbackContext*> custom_recognizers;
    // std::map<std::string, CallbackContext*> custom_actions;
    bool disposed = false;

    void dispose()
    {
        if (disposed) {
            return;
        }
        disposed = true;
        MaaTaskerDestroy(handle);
        if (callback) {
            delete callback;
        }
        // ClearRecos();
        // ClearActs();
    }

    ~TaskerInfo() { dispose(); }

    // void ClearRecos()
    // {
    //     for (const auto& [name, cb] : custom_recognizers) {
    //         delete cb;
    //     }
    //     custom_recognizers.clear();
    // }

    // void ClearActs()
    // {
    //     for (const auto& [name, cb] : custom_actions) {
    //         delete cb;
    //     }
    //     custom_actions.clear();
    // }
};
