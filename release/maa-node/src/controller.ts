import path from 'path'

import { ImageBuffer } from './image'
import maa from './maa'

export class ControllerBase {
    handle: maa.ControllerHandle

    constructor(handle: maa.ControllerHandle) {
        this.handle = handle
    }

    destroy() {
        maa.controller_destroy(this.handle)
    }

    notify(msg: string, detail: string): maa.MaybePromise<void> {}

    post_connection() {
        return maa.controller_wait(this.handle, maa.controller_post_connection(this.handle))
    }

    get connected() {
        return maa.controller_connected(this.handle)
    }

    get uuid() {
        return maa.controller_get_uuid(this.handle)
    }
}

export class AdbController extends ControllerBase {
    constructor(info: maa.AdbInfo, agent?: string) {
        let ws: WeakRef<this>
        const h = maa.adb_controller_create(
            info.adb_path,
            info.adb_serial,
            info.adb_controller_type,
            info.adb_config,
            agent ?? AdbController.agent_path(),
            (msg, detail) => {
                ws.deref()?.notify(msg, detail)
            }
        )
        if (!h) {
            throw 'AdbController create failed'
        }
        super(h)
        ws = new WeakRef(this)
    }

    static agent_path() {
        return path.join(__dirname, '..', 'agent')
    }

    static async find(adb?: string) {
        let ret: boolean
        if (adb) {
            ret = maa.post_find_device_with_adb(adb)
        } else {
            ret = maa.post_find_device()
        }
        if (!ret) {
            throw 'AdbController find failed'
        }
        const size = await maa.wait_for_find_device_to_complete()
        return Array.from({ length: size }, (_, index) => maa.get_device(index))
    }
}

export class Win32Controller extends ControllerBase {
    constructor(hwnd: maa.Win32Hwnd, type: number) {
        let ws: WeakRef<this>
        const h = maa.win32_controller_create(hwnd, type, (msg, detail) => {
            ws.deref()?.notify(msg, detail)
        })
        if (!h) {
            throw 'Win32Controller create failed'
        }
        super(h)
        ws = new WeakRef(this)
    }

    static find(type: 'find' | 'search', class_name?: string, window_name?: string): maa.Win32Hwnd[]
    static find(type: 'list'): maa.Win32Hwnd[]
    static find(type: 'find' | 'search' | 'list', class_name?: string, window_name?: string) {
        let size: number
        switch (type) {
            case 'find':
                size = maa.find_window(class_name ?? '', window_name ?? '')
                break
            case 'search':
                size = maa.search_window(class_name ?? '', window_name ?? '')
                break
            case 'list':
                size = maa.list_windows()
                break
        }
        return Array.from({ length: size }, (_, index) => maa.get_window(index))
    }

    static get(type: 'cursor' | 'desktop' | 'foreground') {
        switch (type) {
            case 'cursor':
                return maa.get_cursor_window()
            case 'desktop':
                return maa.get_desktop_window()
            case 'foreground':
                return maa.get_foreground_window()
        }
    }

    static info(hwnd: maa.Win32Hwnd) {
        return maa.get_window_info(hwnd)
    }
}

export abstract class CustomControllerActor {
    abstract connect(): maa.MaybePromise<boolean>
    abstract request_uuid(): maa.MaybePromise<string | null>
    abstract start_app(intent: string): maa.MaybePromise<boolean>
    abstract stop_app(intent: string): maa.MaybePromise<boolean>
    abstract screencap(image: ImageBuffer): maa.MaybePromise<boolean>
    abstract click(x: number, y: number): maa.MaybePromise<boolean>
    abstract swipe(
        x1: number,
        y1: number,
        x2: number,
        y2: number,
        duration: number
    ): maa.MaybePromise<boolean>
    abstract touch_down(
        contact: number,
        x: number,
        y: number,
        pressure: number
    ): maa.MaybePromise<boolean>
    abstract touch_move(
        contact: number,
        x: number,
        y: number,
        pressure: number
    ): maa.MaybePromise<boolean>
    abstract touch_up(contact: number): maa.MaybePromise<boolean>
    abstract press_key(keycode: number): maa.MaybePromise<boolean>
    abstract input_text(text: string): maa.MaybePromise<boolean>
}

export class CustomControllerActorDefaultImpl extends CustomControllerActor {
    connect(): maa.MaybePromise<boolean> {
        return false
    }
    request_uuid(): maa.MaybePromise<string | null> {
        return null
    }
    start_app(intent: string): maa.MaybePromise<boolean> {
        return false
    }
    stop_app(intent: string): maa.MaybePromise<boolean> {
        return false
    }
    screencap(image: ImageBuffer): maa.MaybePromise<boolean> {
        return false
    }
    click(x: number, y: number): maa.MaybePromise<boolean> {
        return false
    }
    swipe(
        x1: number,
        y1: number,
        x2: number,
        y2: number,
        duration: number
    ): maa.MaybePromise<boolean> {
        return false
    }
    touch_down(contact: number, x: number, y: number, pressure: number): maa.MaybePromise<boolean> {
        return false
    }
    touch_move(contact: number, x: number, y: number, pressure: number): maa.MaybePromise<boolean> {
        return false
    }
    touch_up(contact: number): maa.MaybePromise<boolean> {
        return false
    }
    press_key(keycode: number): maa.MaybePromise<boolean> {
        return false
    }
    input_text(text: string): maa.MaybePromise<boolean> {
        return false
    }
}

export class CustomController extends ControllerBase {
    constructor(actor: CustomControllerActor) {
        let ws: WeakRef<this>
        const h = maa.custom_controller_create(
            (action, ...param) => {
                if (action === 'screencap') {
                    return actor.screencap(new ImageBuffer(param[0]))
                }
                return (actor[action] as any)(...param)
            },
            (msg, detail) => {
                ws.deref()?.notify(msg, detail)
            }
        )
        if (!h) {
            throw 'CustomController create failed'
        }
        super(h)
        ws = new WeakRef(this)
    }
}
