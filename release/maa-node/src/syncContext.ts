import { ImageBuffer } from './image'
import maa from './maa'
import { TaskDecl } from './task'

export class SyncContext {
    handle: maa.SyncContextHandle

    constructor(handle: maa.SyncContextHandle) {
        this.handle = handle
    }

    run_task(name: string, param: Record<string, TaskDecl> = {}) {
        return maa.sync_context_run_task(this.handle, name, JSON.stringify(param))
    }

    run_recognition(name: string, image: ImageBuffer, param: Record<string, TaskDecl> = {}) {
        return maa.sync_context_run_recognition(
            this.handle,
            image.handle,
            name,
            JSON.stringify(param)
        )
    }

    run_action(name: string, box: maa.Rect, detail: string, param: Record<string, TaskDecl> = {}) {
        return maa.sync_context_run_action(this.handle, name, JSON.stringify(param), box, detail)
    }

    click(x: number, y: number) {
        return maa.sync_context_click(this.handle, x, y)
    }

    swipe(x1: number, y1: number, x2: number, y2: number, duration: number) {
        return maa.sync_context_swipe(this.handle, x1, y1, x2, y2, duration)
    }

    press_key(keycode: number) {
        return maa.sync_context_press_key(this.handle, keycode)
    }

    input_text(text: string) {
        return maa.sync_context_input_text(this.handle, text)
    }

    touch_down(contact: number, x: number, y: number, pressure: number) {
        return maa.sync_context_touch_down(this.handle, contact, x, y, pressure)
    }

    touch_move(contact: number, x: number, y: number, pressure: number) {
        return maa.sync_context_touch_move(this.handle, contact, x, y, pressure)
    }

    touch_up(contact: number) {
        return maa.sync_context_touch_up(this.handle, contact)
    }

    screencap(image: ImageBuffer) {
        return maa.sync_context_screencap(this.handle, image.handle)
    }

    cached_image(image: ImageBuffer) {
        return maa.sync_context_cached_image(this.handle, image.handle)
    }
}
