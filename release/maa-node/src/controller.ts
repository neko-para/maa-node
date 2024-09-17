import path from 'path'

import { Job, JobSource } from './job'
import maa from './maa'

export type ControllerNotify =
    | ({
          adb: string
          address: string
      } & (
          | {
                msg: 'UUIDGot'
                uuid: string
            }
          | {
                msg: 'UUIDGetFailed'
            }
          | {
                msg: 'ConnectSuccess'
            }
          | {
                msg: 'ConnectFailed'
                why: 'ConnectFailed' | 'UUIDGetFailed'
            }
          | {
                msg:
                    | 'ScreencapInited'
                    | 'ScreencapInitFailed'
                    | 'TouchinputInited'
                    | 'TouchinputInitFailed'
            }
      ))
    | {
          msg: 'Action.Started' | 'Action.Completed' | 'Action.Failed'
          ctrl_id: maa.CtrlId
          uuid: string
      }

export class ControllerBase {
    handle: maa.ControllerHandle
    #source: JobSource<maa.CtrlId>

    notify(message: string, details_json: string): maa.MaybePromise<void> {}

    set parsed_notify(cb: (msg: ControllerNotify) => maa.MaybePromise<void>) {
        this.notify = (msg, details) => {
            return cb({
                msg: msg.replace(/^Controller\./, '') as any,
                ...JSON.parse(details)
            })
        }
    }

    constructor(handle: maa.ControllerHandle) {
        this.handle = handle
        this.#source = {
            status: id => maa.controller_status(this.handle, id),
            wait: id => maa.controller_wait(this.handle, id)
        }
    }

    destroy() {
        maa.controller_destroy(this.handle)
    }

    set screenshot_target_long_side(value: number) {
        if (!maa.controller_set_option_screenshot_target_long_side(this.handle, value)) {
            throw 'Controller set screenshot_target_long_side failed'
        }
    }

    set screenshot_target_short_side(value: number) {
        if (!maa.controller_set_option_screenshot_target_short_side(this.handle, value)) {
            throw 'Controller set screenshot_target_short_side failed'
        }
    }

    set recording(value: boolean) {
        if (!maa.controller_set_option_recording(this.handle, value)) {
            throw 'Controller set recording failed'
        }
    }

    post_connection() {
        return new Job(this.#source, maa.controller_post_connection(this.handle))
    }

    post_click(x: number, y: number) {
        return new Job(this.#source, maa.controller_post_click(this.handle, x, y))
    }

    post_swipe(x1: number, y1: number, x2: number, y2: number, duration: number) {
        return new Job(
            this.#source,
            maa.controller_post_swipe(this.handle, x1, y1, x2, y2, duration)
        )
    }

    post_press_key(keycode: number) {
        return new Job(this.#source, maa.controller_post_press_key(this.handle, keycode))
    }

    post_input_text(text: string) {
        return new Job(this.#source, maa.controller_post_input_text(this.handle, text))
    }

    post_start_app(intent: string) {
        return new Job(this.#source, maa.controller_post_start_app(this.handle, intent))
    }

    post_stop_app(intent: string) {
        return new Job(this.#source, maa.controller_post_stop_app(this.handle, intent))
    }

    post_touch_down(contact: number, x: number, y: number, pressure: number) {
        return new Job(
            this.#source,
            maa.controller_post_touch_down(this.handle, contact, x, y, pressure)
        )
    }

    post_touch_move(contact: number, x: number, y: number, pressure: number) {
        return new Job(
            this.#source,
            maa.controller_post_touch_move(this.handle, contact, x, y, pressure)
        )
    }

    post_touch_up(contact: number) {
        return new Job(this.#source, maa.controller_post_touch_up(this.handle, contact))
    }

    post_screencap() {
        return new Job(this.#source, maa.controller_post_screencap(this.handle))
    }

    get connected() {
        return maa.controller_connected(this.handle)
    }

    get cached_image() {
        return maa.controller_cached_image(this.handle)
    }

    get uuid() {
        return maa.controller_get_uuid(this.handle)
    }
}

export class AdbController extends ControllerBase {
    constructor(
        adb_path: string,
        address: string,
        screencap_methods: maa.ScreencapOrInputMethods,
        input_methods: maa.ScreencapOrInputMethods,
        config: string,
        agent?: string
    ) {
        let ws: WeakRef<this>
        const h = maa.adb_controller_create(
            adb_path,
            address,
            screencap_methods,
            input_methods,
            config,
            agent ?? AdbController.agent_path(),
            (message, details_json) => {
                ws.deref()?.notify(message, details_json)
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
        return maa.find_adb(adb ?? null)
    }
}

export class Win32Controller extends ControllerBase {
    constructor(
        hwnd: maa.DesktopHandle,
        screencap_methods: maa.ScreencapOrInputMethods,
        input_methods: maa.ScreencapOrInputMethods
    ) {
        let ws: WeakRef<this>
        const h = maa.win32_controller_create(
            hwnd,
            screencap_methods,
            input_methods,
            (message, details_json) => {
                ws.deref()?.notify(message, details_json)
            }
        )
        if (!h) {
            throw 'Win32Controller create failed'
        }
        super(h)
        ws = new WeakRef(this)
    }

    static find() {
        return maa.find_desktop()
    }
}

export class DbgController extends ControllerBase {
    constructor(read_path: string, write_path: string, type: maa.Uint64, config: string) {
        let ws: WeakRef<this>
        const h = maa.dbg_controller_create(
            read_path,
            write_path,
            type,
            config,
            (message, details_json) => {
                ws.deref()?.notify(message, details_json)
            }
        )
        if (!h) {
            throw 'DbgController create failed'
        }
        super(h)
        ws = new WeakRef(this)
    }
}

/*
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
*/
