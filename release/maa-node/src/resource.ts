import { Context } from './context'
import { Job, JobSource } from './job'
import maa from './maa'

export interface CustomRecognizerSelf {
    context: Context
    id: maa.TaskId
    name: string
    param: unknown
    image: maa.ImageData
}

export interface CustomActionSelf {
    context: Context
    id: maa.TaskId
    name: string
    param: unknown
    box: maa.Rect
    detail: string
}

export class ResourceBase {
    handle: maa.ResourceHandle
    #source: JobSource<maa.ResId>

    notify(message: string, details_json: string): maa.MaybePromise<void> {}

    constructor(handle: maa.ResourceHandle) {
        this.handle = handle
        this.#source = {
            status: id => maa.resource_status(this.handle, id),
            wait: id => maa.resource_wait(this.handle, id)
        }
    }

    destroy() {
        maa.resource_destroy(this.handle)
    }

    register_custom_recognizer(
        name: string,
        func: (
            this: CustomRecognizerSelf
        ) => maa.MaybePromise<[out_box: maa.Rect, out_detail: string] | null>
    ) {
        if (
            !maa.resource_register_custom_recognizer(
                this.handle,
                name,
                (context, id, name, param, image) => {
                    const self: CustomRecognizerSelf = {
                        context: new Context(context),
                        id,
                        name,
                        param: JSON.parse(param),
                        image
                    }
                    return func.apply(self)
                }
            )
        ) {
            throw 'Resource register_custom_recognizer failed'
        }
    }

    unregister_custom_recognizer(name: string) {
        if (!maa.resource_unregister_custom_recognizer(this.handle, name)) {
            throw 'Resource unregister_custom_recognizer failed'
        }
    }

    clear_custom_recognizer() {
        if (!maa.resource_clear_custom_recognizer(this.handle)) {
            throw 'Resource clear_custom_recognizer failed'
        }
    }

    register_custom_action(
        name: string,
        func: (this: CustomActionSelf) => maa.MaybePromise<boolean>
    ) {
        if (
            !maa.resource_register_custom_action(
                this.handle,
                name,
                (context, id, name, param, box, detail) => {
                    const self: CustomActionSelf = {
                        context: new Context(context),
                        id,
                        name,
                        param: JSON.parse(param),
                        box,
                        detail
                    }
                    return func.apply(self)
                }
            )
        ) {
            throw 'Resource register_custom_action failed'
        }
    }

    unregister_custom_action(name: string) {
        if (!maa.resource_unregister_custom_action(this.handle, name)) {
            throw 'Resource unregister_custom_action failed'
        }
    }

    clear_custom_action() {
        if (!maa.resource_clear_custom_action(this.handle)) {
            throw 'Resource clear_custom_action failed'
        }
    }

    post_path(path: string) {
        return new Job(this.#source, maa.resource_post_path(this.handle, path))
    }

    clear() {
        if (!maa.resource_clear(this.handle)) {
            throw 'Resource clear failed'
        }
    }

    get loaded() {
        return maa.resource_loaded(this.handle)
    }

    get hash() {
        return maa.resource_get_hash(this.handle)
    }

    get task_list() {
        return maa.resource_get_task_list(this.handle)
    }
}

export class Resource extends ResourceBase {
    constructor() {
        let ws: WeakRef<this>
        const h = maa.resource_create((message, details_json) => {
            ws.deref()?.notify(message, details_json)
        })
        if (!h) {
            throw 'Resource create failed'
        }
        super(h)
        ws = new WeakRef(this)
    }
}
