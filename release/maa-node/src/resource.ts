import maa from './maa'

export class ResourceBase {
    handle: maa.ResourceHandle

    constructor(handle: maa.ResourceHandle) {
        this.handle = handle
    }

    notify(msg: string, detail: string): maa.MaybePromise<void> {}

    post_path(path: string): Promise<maa.Status> {
        return maa.resource_wait(this.handle, maa.resource_post_path(this.handle, path))
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
        const h = maa.resource_create((msg, detail) => {
            ws.deref()?.notify(msg, detail)
        })
        if (!h) {
            throw 'Resource create failed'
        }
        super(h)
        ws = new WeakRef(this)
    }
}
