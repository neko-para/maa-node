import { ControllerBase } from './controller'
import { Job, JobSource } from './job'
import maa from './maa'
import { ResourceBase } from './resource'

export class TaskerBase {
    handle: maa.TaskerHandle
    #source: JobSource<maa.TaskId>

    notify(message: string, details_json: string): maa.MaybePromise<void> {}

    constructor(handle: maa.TaskerHandle) {
        this.handle = handle
        this.#source = {
            status: id => maa.tasker_status(this.handle, id),
            wait: id => maa.tasker_wait(this.handle, id)
        }
    }

    destroy() {
        maa.tasker_destroy(this.handle)
    }

    bind(slave: ControllerBase | ResourceBase) {
        let ret: boolean
        if (slave instanceof ControllerBase) {
            ret = maa.tasker_bind_controller(this.handle, slave.handle)
        } else {
            ret = maa.tasker_bind_resource(this.handle, slave.handle)
        }
        if (!ret) {
            throw 'Tasker bind failed'
        }
    }

    post_pipeline(entry: string, param: Record<string, unknown> = {}) {
        return new Job(
            this.#source,
            maa.tasker_post_pipeline(this.handle, entry, JSON.stringify(param))
        )
    }

    post_recognition(entry: string, param: Record<string, unknown> = {}) {
        return new Job(
            this.#source,
            maa.tasker_post_recognition(this.handle, entry, JSON.stringify(param))
        )
    }

    post_action(entry: string, param: Record<string, unknown> = {}) {
        return new Job(
            this.#source,
            maa.tasker_post_action(this.handle, entry, JSON.stringify(param))
        )
    }

    get inited() {
        return maa.tasker_inited(this.handle)
    }

    get running() {
        return maa.tasker_running(this.handle)
    }

    post_stop() {
        if (!maa.tasker_post_stop(this.handle)) {
            throw 'Tasker post_stop failed'
        }
    }
}

export class Tasker extends TaskerBase {
    constructor() {
        let ws: WeakRef<this>
        const h = maa.tasker_create((msg, detail) => {
            ws.deref()?.notify(msg, detail)
        })
        if (!h) {
            throw 'Tasker create failed'
        }
        super(h)
        ws = new WeakRef(this)
    }
}
