export type ControllerHandle = { __brand: 'ControllerHandle' }
export type ImageBufferHandle = { __brand: 'ImageBufferHandle' }

export type CtrlId = number & { __brand: 'CtrlId' }

export type TrivialCallbak = (msg: string, details: string) => void | Promise<void>

export function adb_controller_create(
    adb_path: string,
    adb_serial: string,
    adb_controller_type: number,
    adb_config: string,
    agent_path: string,
    callback: TrivialCallbak
): ControllerHandle | null
export function controller_post_connection(handle: ControllerHandle): CtrlId
export function controller_wait(
    handle: ControllerHandle,
    ctrlId: CtrlId,
    cb: (err?: Error, result?: number) => void
): void

export function version(): string

export function create_image_buffer(): ImageBufferHandle | null
export function is_image_empty(handle: ImageBufferHandle): boolean

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
