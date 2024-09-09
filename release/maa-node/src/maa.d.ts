export type ResourceHandle = { __brand: 'ResourceHandle' }
export type ControllerHandle = { __brand: 'ControllerHandle' }
export type TaskerHandle = { __brand: 'TaskerHandle' }
export type ContextHandle = { __brand: 'ContextHandle' }
export type DesktopHandle = { __brand: 'DesktopHandle' }

type Id = number | bigint
type ScreencapOrInputMethods = number | bigint

export type ResId = Id & { __brand: 'ResId' }
export type CtrlId = Id & { __brand: 'CtrlId' }
export type TaskId = Id & { __brand: 'TaskId' }
export type RecoId = Id & { __brand: 'RecoId' }
export type NodeId = Id & { __brand: 'NodeId' }

export type ImageData = ArrayBuffer

export type Rect = {
    x: number
    y: number
    width: number
    height: number
}

type MaybePromise<T> = T | Promise<T>

export type NotificationCallbak = (message: string, details_json: string) => MaybePromise<void>
export type CustomRecognizerCallback = (
    context: ContextHandle,
    task_id: TaskId,
    recognizer_name: string,
    custom_recognition_param: string,
    image: ImageData
) => MaybePromise<[out_box: Rect, out_detail: string] | null>
export type CustomActionCallback = (
    context: ContextHandle,
    task_id: TaskId,
    action_name: string,
    custom_action_param: string,
    box: Rect,
    reco_detail: string
) => MaybePromise<boolean>

// context.cpp

export declare function context_run_pipeline(
    context: ContextHandle,
    entry: string,
    pipeline_override: string
): TaskId
export declare function context_run_recognition(
    context: ContextHandle,
    entry: string,
    pipeline_override: string,
    image: ImageData
): RecoId
export declare function context_run_action(
    context: ContextHandle,
    entry: string,
    pipeline_override: string,
    box: Rect,
    reco_detail: string
): NodeId
export declare function context_override_pipeline(
    context: ContextHandle,
    pipeline_override: string
): boolean
export declare function context_get_task_id(context: ContextHandle): TaskId
export declare function context_get_tasker(context: ContextHandle): TaskerHandle
export declare function context_clone(context: ContextHandle): ContextHandle

// controller.cpp

export declare function adb_controller_create(
    adb_path: string,
    address: string,
    screencap_methods: ScreencapOrInputMethods,
    input_methods: ScreencapOrInputMethods,
    config: string,
    agent_path: string,
    callback: NotificationCallbak | null
): ControllerHandle | null
export declare function win32_controller_create(
    handle: DesktopHandle,
    screencap_methods: ScreencapOrInputMethods,
    input_methods: ScreencapOrInputMethods,
    callback: NotificationCallbak | null
): ControllerHandle | null
// export declare function custom_controller_create(
//     custom_callback: CustomControllerCallback,
//     callback: NotificationCallbak | null
// ): ControllerHandle | null
export declare function controller_destroy(handle: ControllerHandle): void
export declare function controller_set_option_screenshot_target_long_side(
    handle: ControllerHandle,
    value: number
): boolean
export declare function controller_set_option_screenshot_target_short_side(
    handle: ControllerHandle,
    value: number
): boolean
export declare function controller_set_option_default_app_package_entry(
    handle: ControllerHandle,
    value: string
): boolean
export declare function controller_set_option_default_app_package(
    handle: ControllerHandle,
    value: string
): boolean
export declare function controller_set_option_recording(
    handle: ControllerHandle,
    value: boolean
): boolean
export declare function controller_post_connection(handle: ControllerHandle): CtrlId
// export declare function controller_post_click(
//     handle: ControllerHandle,
//     x: number,
//     y: number
// ): CtrlId
// export declare function controller_post_swipe(
//     handle: ControllerHandle,
//     x1: number,
//     y1: number,
//     x2: number,
//     y2: number,
//     duration: number
// ): CtrlId
// export declare function controller_post_press_key(handle: ControllerHandle, keycode: number): CtrlId
// export declare function controller_post_input_text(handle: ControllerHandle, text: string): CtrlId
// export declare function controller_post_start_app(handle: ControllerHandle, intent: string): CtrlId
// export declare function controller_post_stop_app(handle: ControllerHandle, intent: string): CtrlId
// export declare function controller_post_touch_down(
//     handle: ControllerHandle,
//     contact: number,
//     x: number,
//     y: number,
//     pressure: number
// ): CtrlId
// export declare function controller_post_touch_move(
//     handle: ControllerHandle,
//     contact: number,
//     x: number,
//     y: number,
//     pressure: number
// ): CtrlId
// export declare function controller_post_touch_up(handle: ControllerHandle, contact: number): CtrlId
export declare function controller_post_screencap(handle: ControllerHandle): CtrlId
export declare function controller_status(handle: ControllerHandle, ctrl_id: CtrlId): Status
export declare function controller_wait(handle: ControllerHandle, ctrl_id: CtrlId): Promise<Status>
export declare function controller_connected(handle: ControllerHandle): boolean
export declare function controller_cached_image(handle: ControllerHandle): ImageData | null
export declare function controller_get_uuid(handle: ControllerHandle): string | null

// resource.cpp

export declare function resource_create(callback: NotificationCallbak | null): ResourceHandle | null
export declare function resource_destroy(handle: ResourceHandle): void
export declare function resource_register_custom_recognizer(
    handle: ResourceHandle,
    name: string,
    recognizer: CustomRecognizerAnalyzeCallback
): boolean
export declare function resource_unregister_custom_recognizer(
    handle: ResourceHandle,
    name: string
): boolean
export declare function resource_clear_custom_recognizer(handle: ResourceHandle): boolean
export declare function resource_register_custom_action(
    handle: ResourceHandle,
    name: string,
    action: CustomActionRunCallback
): boolean
export declare function resource_unregister_custom_action(
    handle: ResourceHandle,
    name: string
): boolean
export declare function resource_clear_custom_action(handle: ResourceHandle): boolean
export declare function resource_post_path(handle: ResourceHandle, path: string): ResId
export declare function resource_clear(handle: ResourceHandle): boolean
export declare function resource_status(handle: ResourceHandle, res_id: ResId): Status
export declare function resource_wait(handle: ResourceHandle, res_id: ResId): Promise<Status>
export declare function resource_loaded(handle: ResourceHandle): boolean
export declare function resource_get_hash(handle: ResourceHandle): string | null
export declare function resource_get_task_list(handle: ResourceHandle): string[] | null

// tasker.cpp

export declare function tasker_create(callback: NotificationCallbak | null): TaskerHandle | null
export declare function tasker_destroy(handle: TaskerHandle): void
export declare function tasker_bind_resource(
    handle: TaskerHandle,
    resource: ResourceHandle | null
): boolean
export declare function tasker_bind_controller(
    handle: TaskerHandle,
    controller: ControllerHandle | null
): boolean
export declare function tasker_inited(handle: TaskerHandle): boolean
export declare function tasker_post_pipeline(
    handle: TaskerHandle,
    entry: string,
    pipeline_override: string
): TaskId
export declare function tasker_post_recognition(
    handle: TaskerHandle,
    entry: string,
    pipeline_override: string
): TaskId
export declare function tasker_post_action(
    handle: TaskerHandle,
    entry: string,
    pipeline_override: string
): TaskId
export declare function tasker_status(handle: TaskerHandle, task_id: TaskId): Status
export declare function tasker_wait(handle: TaskerHandle, task_id: TaskId): Promise<Status>
export declare function tasker_running(handle: TaskerHandle): boolean
export declare function tasker_post_stop(handle: TaskerHandle): boolean
export declare function tasker_get_resource(handle: TaskerHandle): ResourceHandle | null
export declare function tasker_get_controller(handle: TaskerHandle): ControllerHandle | null
export declare function tasker_clear_cache(handle: TaskerHandle): boolean
export declare function tasker_get_recognition_detail(
    handle: TaskerHandle,
    reco_id: RecoId
):
    | [name: string, hit: boolean, box: Rect, detail: string, raw: ImageData, draws: ImageData[]]
    | null
export declare function tasker_get_node_detail(
    handle: TaskerHandle,
    node_id: NodeId
): [name: string, reco_id: RecoId, times: number, completed: boolean] | null
export declare function tasker_get_task_detail(
    handle: TaskerHandle,
    task_id: TaskId
): [entry: string, node_ids: NodeId[]] | null
export declare function tasker_get_latest_node(
    handle: TaskerHandle,
    task_name: string
): NodeId | null

// config.cpp

export declare function config_init_option(user_path: string, default_json: string): boolean

// find.cpp

export declare function find_adb(
    adb_path: string | null
): Promise<
    | [
          name: string,
          adb_path: string,
          address: string,
          screencap_methods: ScreencapOrInputMethods,
          input_methods: ScreencapOrInputMethods,
          config: string
      ][]
    | null
>
export declare function find_desktop(): Promise<
    [handle: DesktopHandle, class_name: string, window_name: string][] | null
>

// utility.cpp

export declare function version(): string
export declare function set_global_option_log_dir(value: string): boolean
export declare function set_global_option_save_draw(value: boolean): boolean
export declare function set_global_option_recording(value: boolean): boolean
export declare function set_global_option_stdout_level(value: number): boolean
export declare function set_global_option_show_hit_draw(value: boolean): boolean
export declare function set_global_option_debug_message(value: boolean): boolean

// export declare function unwrap_window_hwnd(hwnd: Win32Hwnd): string
// export declare function wrap_window_hwnd(hwnd: string): Win32Hwnd | null

export declare const Status: Record<
    'Invalid' | 'Pending' | 'Running' | 'Success' | 'Failed',
    Status
>
export declare const LoggingLevel: Record<
    'Off' | 'Fatal' | 'Error' | 'Warn' | 'Info' | 'Debug' | 'Trace' | 'All',
    number
>
export declare const AdbControllerType: Record<
    | 'Touch_Adb'
    | 'Touch_MiniTouch'
    | 'Touch_MaaTouch'
    | 'Touch_EmulatorExtras'
    | 'Touch_AutoDetect'
    | 'Key_Adb'
    | 'Key_MaaTouch'
    | 'Key_EmulatorExtras'
    | 'Key_AutoDetect'
    | 'Input_Preset_Adb'
    | 'Input_Preset_MiniTouch'
    | 'Input_Preset_MaaTouch'
    | 'Input_Preset_AutoDetect'
    | 'Input_Preset_EmulatorExtras'
    | 'Screencap_FastestWay_Compatible'
    | 'Screencap_RawByNetcat'
    | 'Screencap_RawWithGzip'
    | 'Screencap_Encode'
    | 'Screencap_EncodeToFile'
    | 'Screencap_MinicapDirect'
    | 'Screencap_MinicapStream'
    | 'Screencap_EmulatorExtras'
    | 'Screencap_FastestLosslessWay'
    | 'Screencap_FastestWay',
    number
>
export declare const Win32ControllerType: Record<
    | 'Touch_SendMessage'
    | 'Touch_Seize'
    | 'Key_SendMessage'
    | 'Key_Seize'
    | 'Screencap_GDI'
    | 'Screencap_DXGI_DesktopDup'
    | 'Screencap_DXGI_FramePool',
    number
>
