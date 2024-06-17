export type ImageBufferHandle = { __brand: 'ImageBufferHandle' }

export function version(): string

export function create_image_buffer(): ImageBufferHandle | null
export function is_image_empty(handle: ImageBufferHandle): boolean

export function post_find_device(): boolean
export function post_find_device_with_adb(path: string): boolean
export function is_find_device_completed(): boolean
export function wait_for_find_device_to_complete(cb: (err?: Error, result?: number) => void): void
