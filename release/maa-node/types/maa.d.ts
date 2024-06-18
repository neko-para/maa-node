export type ControllerHandle = { __brand: 'ControllerHandle' }
export type InstanceHandle = { __brand: 'InstanceHandle' }
export type ResourceHandle = { __brand: 'ResourceHandle' }
export type SyncContextHandle = { __brand: 'SyncContextHandle' }
export type ImageBufferHandle = { __brand: 'ImageBufferHandle' }
export type ImageListBufferHandle = { __brand: 'ImageListBufferHandle' }

export type CtrlId = number & { __brand: 'CtrlId' }
export type TaskId = number & { __brand: 'TaskId' }
export type ResId = number & { __brand: 'ResId' }
export type RecoId = number & { __brand: 'RecoId' }
export type NodeId = number & { __brand: 'NodeId' }

export type Rect = {
    x: number
    y: number
    width: number
    height: number
}

export type TrivialCallbak = (msg: string, details: string) => void | Promise<void>
export type CustomActionRunCallback = (
    sync_context: SyncContextHandle,
    task_name: string,
    custom_action_param: string,
    cur_box: Rect,
    cur_rec_detail: string
) => boolean | Promise<boolean>

export function adb_controller_create(
    adb_path: string,
    adb_serial: string,
    adb_controller_type: number,
    adb_config: string,
    agent_path: string,
    callback: TrivialCallbak | null
): ControllerHandle | null
export function set_controller_option(
    handle: ControllerHandle,
    key: 'ScreenshotTargetLongSide',
    value: number
): boolean
export function set_controller_option(
    handle: ControllerHandle,
    key: 'ScreenshotTargetShortSide',
    value: number
): boolean
export function set_controller_option(
    handle: ControllerHandle,
    key: 'DefaultAppPackageEntry',
    value: string
): boolean
export function set_controller_option(
    handle: ControllerHandle,
    key: 'DefaultAppPackage',
    value: string
): boolean
export function set_controller_option(
    handle: ControllerHandle,
    key: 'Recording',
    value: boolean
): boolean
export function controller_post_connection(handle: ControllerHandle): CtrlId
export function controller_post_screencap(handle: ControllerHandle): CtrlId
export function controller_status(handle: ControllerHandle): number
export function controller_wait(
    handle: ControllerHandle,
    ctrl_id: CtrlId,
    cb: (err?: Error, result?: number) => void
): void
export function controller_connected(handle: ControllerHandle): boolean
export function controller_get_image(handle: ControllerHandle, buffer: ImageBufferHandle): boolean
export function controller_get_uuid(handle: ControllerHandle): string | null

export function create(callback: TrivialCallbak | null): InstanceHandle | null
export function bind_resource(handle: InstanceHandle, resource: ResourceHandle): boolean
export function bind_controller(handle: InstanceHandle, controller: ControllerHandle): boolean
export function inited(handle: InstanceHandle): boolean
export function register_custom_action(
    handle: InstanceHandle,
    name: string,
    func: CustomActionRunCallback
): boolean
export function post_task(handle: InstanceHandle, entry: string, param: string): TaskId
export function post_recognition(handle: InstanceHandle, entry: string, param: string): TaskId
export function post_action(handle: InstanceHandle, entry: string, param: string): TaskId
export function set_task_param(handle: InstanceHandle, task_id: TaskId, param: string): boolean
export function task_status(handle: InstanceHandle): number
export function wait_task(
    handle: InstanceHandle,
    task_id: TaskId,
    cb: (err?: Error, result?: number) => void
): void
export function running(handle: InstanceHandle): boolean
export function post_stop(handle: InstanceHandle): boolean

export function resource_create(callback: TrivialCallbak | null): ResourceHandle | null
export function resource_post_path(handle: ResourceHandle, path: string): ResId
export function resource_clear(handle: ResourceHandle): boolean
export function resource_status(handle: ResourceHandle): number
export function resource_wait(
    handle: ResourceHandle,
    res_id: ResId,
    cb: (err?: Error, result?: number) => void
): void
export function resource_loaded(handle: ResourceHandle): boolean
export function resource_get_hash(handle: ResourceHandle): string | null
export function resource_get_task_list(handle: ResourceHandle): string | null

export function version(): string
export function set_global_option(key: 'LogDir', value: string): boolean
export function set_global_option(key: 'SaveDraw', value: boolean): boolean
export function set_global_option(key: 'Recording', value: boolean): boolean
export function set_global_option(key: 'StdoutLevel', value: number): boolean
export function set_global_option(key: 'ShowHitDraw', value: boolean): boolean
export function set_global_option(key: 'DebugMessage', value: boolean): boolean
export function query_recognition_detail(
    reco_id: RecoId,
    raw: ImageBufferHandle | null,
    draws: ImageListBufferHandle | null
): {
    name: string
    hit: boolean
    hit_box: Rect
    detail_json: string
} | null
export function query_node_detail(node_id: NodeId): {
    name: string
    reco_id: RecoId
    run_completed: boolean
} | null
export function query_task_detail(task_id: TaskId): {
    entry: string
    node_id_list: NodeId[]
} | null

export function create_image_buffer(): ImageBufferHandle | null
export function is_image_empty(handle: ImageBufferHandle): boolean
export function clear_image(handle: ImageBufferHandle): boolean
export function get_image_info(handle: ImageBufferHandle): {
    width: number
    height: number
    type: number
}
// returned buffer is not copied, thus will become invalid after handle being mutated or destroyed
export function get_image_encoded(handle: ImageBufferHandle): ArrayBuffer
export function set_image_encoded(handle: ImageBufferHandle, data: ArrayBuffer): boolean
export function create_image_list_buffer(): ImageListBufferHandle | null
export function is_image_list_empty(handle: ImageListBufferHandle): boolean
export function clear_image_list(handle: ImageListBufferHandle): boolean
export function get_image_list_size(handle: ImageListBufferHandle): number
export function get_image_list_at(handle: ImageListBufferHandle): ImageBufferHandle
export function image_list_append(handle: ImageListBufferHandle, value: ImageBufferHandle): boolean
export function image_list_remove(handle: ImageListBufferHandle, index: number): boolean

export function post_find_device(): boolean
export function post_find_device_with_adb(path: string): boolean
export function is_find_device_completed(): boolean
export function wait_for_find_device_to_complete(cb: (err?: Error, result?: number) => void): void
export function get_device_count(): number
export function get_device(index: number): {
    name: string
    adb_path: string
    adb_serial: string
    adb_controller_type: number
    adb_config: string
}
