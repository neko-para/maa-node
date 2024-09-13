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

    get resource() {
        const res = maa.tasker_get_resource(this.handle)
        if (res) {
            return new ResourceBase(res)
        } else {
            return null
        }
    }

    get controller() {
        const ctrl = maa.tasker_get_controller(this.handle)
        if (ctrl) {
            return new ControllerBase(ctrl)
        } else {
            return null
        }
    }

    clear_cache() {
        if (!maa.tasker_clear_cache(this.handle)) {
            throw 'Tasker clear_cache failed'
        }
    }

    recognition_detail(id: maa.RecoId) {
        const dt = maa.tasker_get_recognition_detail(this.handle, id)
        if (dt) {
            const [name, algorithm, hit, box, detail, raw, draws] = dt
            return {
                name,
                algorithm,
                hit,
                box,
                detail,
                raw,
                draws
            }
        } else {
            return null
        }
    }

    node_detail(id: maa.NodeId) {
        const dt = maa.tasker_get_node_detail(this.handle, id)
        if (dt) {
            const [name, reco_id, times, completed] = dt
            return {
                name,
                reco: this.recognition_detail(reco_id),
                times,
                completed
            }
        } else {
            return null
        }
    }

    task_detail(id: maa.TaskId) {
        const dt = maa.tasker_get_task_detail(this.handle, id)
        if (dt) {
            const [entry, node_ids] = dt
            return {
                entry,
                nodes: node_ids.map(i => this.node_detail(i))
            }
        } else {
            return null
        }
    }
}

export class Tasker extends TaskerBase {
    constructor() {
        let ws: WeakRef<this>
        const h = maa.tasker_create((message, details_json) => {
            ws.deref()?.notify(message, details_json)
        })
        if (!h) {
            throw 'Tasker create failed'
        }
        super(h)
        ws = new WeakRef(this)
    }
}
