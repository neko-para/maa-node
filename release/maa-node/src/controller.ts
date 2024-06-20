import maa from './maa'

export class ControllerBase {
    handle: maa.ControllerHandle

    constructor(handle: maa.ControllerHandle) {
        this.handle = handle
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
    constructor(info: maa.AdbInfo, agent_path: string) {
        let ws: WeakRef<this>
        const h = maa.adb_controller_create(
            info.adb_path,
            info.adb_serial,
            info.adb_controller_type,
            info.adb_config,
            agent_path,
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
