import { Job, JobSource } from './job'
import maa from './maa'

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
