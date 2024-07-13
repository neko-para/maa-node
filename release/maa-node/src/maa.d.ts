export type ControllerHandle = { __brand: 'ControllerHandle' }
export type InstanceHandle = { __brand: 'InstanceHandle' }
export type ResourceHandle = { __brand: 'ResourceHandle' }
export type SyncContextHandle = { __brand: 'SyncContextHandle' }
export type ImageBufferHandle = { __brand: 'ImageBufferHandle' }
export type ImageListBufferHandle = { __brand: 'ImageListBufferHandle' }
export type Win32Hwnd = { __brand: 'Win32Hwnd' }

export type CtrlId = number & { __brand: 'CtrlId' }
export type TaskId = number & { __brand: 'TaskId' }
export type ResId = number & { __brand: 'ResId' }
export type RecoId = number & { __brand: 'RecoId' }
export type NodeId = number & { __brand: 'NodeId' }

export type Status = number & { __brand: 'Status' }

export type Rect = {
    x: number
    y: number
    width: number
    height: number
}

export type FlatRect = [x: number, y: number, width: number, height: number]

export type AdbInfo = {
    adb_path: string
    adb_serial: string
    adb_controller_type: number
    adb_config: string
}

type MaybePromise<T> = T | Promise<T>

export type TrivialCallbak = (msg: string, details: string) => MaybePromise<void>
export type CustomRecognizerAnalyzeCallback = (
    sync_context: SyncContextHandle,
    image: ImageBufferHandle,
    task_name: string,
    custom_recognition_param: string
) => MaybePromise<{
    out_box: Rect
    out_detail: string
} | null>
export type CustomActionRunCallback = (
    sync_context: SyncContextHandle,
    task_name: string,
    custom_action_param: string,
    cur_box: Rect,
    cur_rec_detail: string
) => MaybePromise<boolean>
export type CustomControllerParamResultMap = {
    connect: [[], boolean]
    request_uuid: [[], string | null]
    request_resolution: [[], { width: number; height: number } | null]
    start_app: [[intent: string], boolean]
    screencap: [[intent: string], boolean]
    screencap: [[image: ImageBufferHandle], boolean]
    click: [[x: number, y: number], boolean]
    swipe: [[x1: number, y1: number, x2: number, y2: number, duration: number], boolean]
    touch_down: [[contact: number, x: number, y: number, pressure: number], boolean]
    touch_move: [[contact: number, x: number, y: number, pressure: number], boolean]
    touch_up: [[contact: number], boolean]
    press_key: [[keycode: number], boolean]
    input_text: [[text: string], boolean]
}
export type CustomControllerCallback = (
    action: keyof CustomControllerParamResultMap,
    ...param: any[]
) => MaybePromise<any>

export declare function adb_controller_create(
    adb_path: string,
    address: string,
    type: number,
    config: string,
    agent_path: string,
    callback: TrivialCallbak | null
): ControllerHandle | null
export declare function win32_controller_create(
    hwnd: Win32Hwnd,
    type: number,
    callback: TrivialCallbak | null
): ControllerHandle | null
export declare function custom_controller_create(
    custom_callback: CustomControllerCallback,
    callback: TrivialCallbak | null
): ControllerHandle | null
export declare function controller_destroy(handle: ControllerHandle): void
export declare function set_controller_option(
    handle: ControllerHandle,
    key: 'ScreenshotTargetLongSide',
    value: number
): boolean
export declare function set_controller_option(
    handle: ControllerHandle,
    key: 'ScreenshotTargetShortSide',
    value: number
): boolean
export declare function set_controller_option(
    handle: ControllerHandle,
    key: 'DefaultAppPackageEntry',
    value: string
): boolean
export declare function set_controller_option(
    handle: ControllerHandle,
    key: 'DefaultAppPackage',
    value: string
): boolean
export declare function set_controller_option(
    handle: ControllerHandle,
    key: 'Recording',
    value: boolean
): boolean
export declare function controller_post_connection(handle: ControllerHandle): CtrlId
export declare function controller_post_click(
    handle: ControllerHandle,
    x: number,
    y: number
): CtrlId
export declare function controller_post_swipe(
    handle: ControllerHandle,
    x1: number,
    y1: number,
    x2: number,
    y2: number,
    duration: number
): CtrlId
export declare function controller_post_press_key(handle: ControllerHandle, keycode: number): CtrlId
export declare function controller_post_input_text(handle: ControllerHandle, text: string): CtrlId
export declare function controller_post_start_app(handle: ControllerHandle, intent: string): CtrlId
export declare function controller_post_stop_app(handle: ControllerHandle, intent: string): CtrlId
export declare function controller_post_touch_down(
    handle: ControllerHandle,
    contact: number,
    x: number,
    y: number,
    pressure: number
): CtrlId
export declare function controller_post_touch_move(
    handle: ControllerHandle,
    contact: number,
    x: number,
    y: number,
    pressure: number
): CtrlId
export declare function controller_post_touch_up(handle: ControllerHandle, contact: number): CtrlId
export declare function controller_post_screencap(handle: ControllerHandle): CtrlId
export declare function controller_status(handle: ControllerHandle): Status
export declare function controller_wait(handle: ControllerHandle, ctrl_id: CtrlId): Promise<Status>
export declare function controller_connected(handle: ControllerHandle): boolean
export declare function controller_get_image(
    handle: ControllerHandle,
    buffer: ImageBufferHandle
): boolean
export declare function controller_get_uuid(handle: ControllerHandle): string | null

export declare function create(callback: TrivialCallbak | null): InstanceHandle | null
export declare function destroy(handle: InstanceHandle): void
export declare function bind_resource(handle: InstanceHandle, resource: ResourceHandle): boolean
export declare function bind_controller(
    handle: InstanceHandle,
    controller: ControllerHandle
): boolean
export declare function inited(handle: InstanceHandle): boolean
export declare function register_custom_recognizer(
    handle: InstanceHandle,
    name: string,
    func: CustomRecognizerAnalyzeCallback
): boolean
export declare function unregister_custom_recognizer(handle: InstanceHandle, name: string): boolean
export declare function clear_custom_recognizer(handle: InstanceHandle): boolean
export declare function register_custom_action(
    handle: InstanceHandle,
    name: string,
    func: CustomActionRunCallback
): boolean
export declare function unregister_custom_action(handle: InstanceHandle, name: string): boolean
export declare function clear_custom_action(handle: InstanceHandle): boolean
export declare function post_task(handle: InstanceHandle, entry: string, param: string): TaskId
export declare function post_recognition(
    handle: InstanceHandle,
    entry: string,
    param: string
): TaskId
export declare function post_action(handle: InstanceHandle, entry: string, param: string): TaskId
export declare function set_task_param(
    handle: InstanceHandle,
    task_id: TaskId,
    param: string
): boolean
export declare function task_status(handle: InstanceHandle): Status
export declare function wait_task(handle: InstanceHandle, task_id: TaskId): Promise<Status>
export declare function running(handle: InstanceHandle): boolean
export declare function post_stop(handle: InstanceHandle): boolean

export declare function resource_create(callback: TrivialCallbak | null): ResourceHandle | null
export declare function resource_destroy(handle: ResourceHandle): void
export declare function resource_post_path(handle: ResourceHandle, path: string): ResId
export declare function resource_clear(handle: ResourceHandle): boolean
export declare function resource_status(handle: ResourceHandle): Status
export declare function resource_wait(handle: ResourceHandle, res_id: ResId): Promise<Status>
export declare function resource_loaded(handle: ResourceHandle): boolean
export declare function resource_get_hash(handle: ResourceHandle): string | null
export declare function resource_get_task_list(handle: ResourceHandle): string | null

export declare function sync_context_run_task(
    handle: SyncContextHandle,
    task_name: string,
    param: string
): Promise<boolean>
export declare function sync_context_run_recognition(
    handle: SyncContextHandle,
    image: ImageBufferHandle,
    task_name: string,
    task_param: string
): Promise<{
    out_box: Rect
    out_detail: string
} | null>
export declare function sync_context_run_action(
    handle: SyncContextHandle,
    task_name: string,
    task_param: string,
    cur_box: Rect,
    cur_rec_detail: string
): Promise<boolean>
export declare function sync_context_click(
    handle: SyncContextHandle,
    x: number,
    y: number
): Promise<boolean>
export declare function sync_context_swipe(
    handle: SyncContextHandle,
    x1: number,
    y1: number,
    x2: number,
    y2: number,
    duration: number
): Promise<boolean>
export declare function sync_context_press_key(
    handle: SyncContextHandle,
    keycode: number
): Promise<boolean>
export declare function sync_context_input_text(
    handle: SyncContextHandle,
    text: string
): Promise<boolean>
export declare function sync_context_touch_down(
    handle: SyncContextHandle,
    contact: number,
    x: number,
    y: number,
    pressure: number
): Promise<boolean>
export declare function sync_context_touch_move(
    handle: SyncContextHandle,
    contact: number,
    x: number,
    y: number,
    pressure: number
): Promise<boolean>
export declare function sync_context_touch_up(
    handle: SyncContextHandle,
    contact: number
): Promise<boolean>
export declare function sync_context_screencap(
    handle: SyncContextHandle,
    image: ImageBufferHandle
): Promise<boolean>
export declare function sync_context_cached_image(
    handle: SyncContextHandle,
    image: ImageBufferHandle
): Promise<boolean>

export declare function version(): string
export declare function set_global_option(key: 'LogDir', value: string): boolean
export declare function set_global_option(key: 'SaveDraw', value: boolean): boolean
export declare function set_global_option(key: 'Recording', value: boolean): boolean
export declare function set_global_option(key: 'StdoutLevel', value: number): boolean
export declare function set_global_option(key: 'ShowHitDraw', value: boolean): boolean
export declare function set_global_option(key: 'DebugMessage', value: boolean): boolean
export declare function query_recognition_detail(
    reco_id: RecoId,
    raw: ImageBufferHandle | null,
    draws: ImageListBufferHandle | null
): {
    name: string
    hit: boolean
    hit_box: Rect
    detail_json: string
} | null
export declare function query_node_detail(node_id: NodeId): {
    name: string
    reco_id: RecoId
    run_completed: boolean
} | null
export declare function query_task_detail(task_id: TaskId): {
    entry: string
    node_id_list: NodeId[]
} | null

export declare function create_image_buffer(): ImageBufferHandle | null
export declare function is_image_empty(handle: ImageBufferHandle): boolean
export declare function clear_image(handle: ImageBufferHandle): boolean
export declare function get_image_info(handle: ImageBufferHandle): {
    width: number
    height: number
    type: number
}
export declare function get_image_encoded(handle: ImageBufferHandle): ArrayBuffer
export declare function set_image_encoded(
    handle: ImageBufferHandle,
    data: ArrayBuffer | Buffer
): boolean
export declare function create_image_list_buffer(): ImageListBufferHandle | null
export declare function is_image_list_empty(handle: ImageListBufferHandle): boolean
export declare function clear_image_list(handle: ImageListBufferHandle): boolean
export declare function get_image_list_size(handle: ImageListBufferHandle): number
export declare function get_image_list_at(
    handle: ImageListBufferHandle,
    index: number
): ImageBufferHandle
export declare function image_list_append(
    handle: ImageListBufferHandle,
    value: ImageBufferHandle
): boolean
export declare function image_list_remove(handle: ImageListBufferHandle, index: number): boolean

export declare function init_option_config(user_path: string, default_json: string): boolean

export declare function post_find_device(): boolean
export declare function post_find_device_with_adb(path: string): boolean
export declare function is_find_device_completed(): boolean
export declare function wait_for_find_device_to_complete(): Promise<number>
export declare function get_device_count(): number
export declare function get_device(index: number): {
    name: string
} & AdbInfo

export declare function register_custom_recognizer_executor(
    handle: InstanceHandle,
    recognizer_name: string,
    exec_path: string,
    exec_params: string[]
): boolean
export declare function unregister_custom_recognizer_executor(
    handle: InstanceHandle,
    recognizer_name: string
): boolean
export declare function clear_custom_recognizer_executor(handle: InstanceHandle): boolean
export declare function register_custom_action_executor(
    handle: InstanceHandle,
    action_name: string,
    exec_path: string,
    exec_params: string[]
): boolean
export declare function unregister_custom_action_executor(
    handle: InstanceHandle,
    action_name: string
): boolean
export declare function clear_custom_action_executor(handle: InstanceHandle): boolean

export declare function find_window(class_name: string, window_name: string): number
export declare function search_window(class_name: string, window_name: string): number
export declare function list_windows(): number
export declare function get_window(index: number): Win32Hwnd
export declare function get_cursor_window(): Win32Hwnd
export declare function get_desktop_window(): Win32Hwnd
export declare function get_foreground_window(): Win32Hwnd
export declare function get_window_info(hwnd: Win32Hwnd): {
    class_name?: string
    window_name?: string
}
export declare function get_window_hwnd(hwnd: Win32Hwnd): string

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
