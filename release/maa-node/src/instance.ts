import { ControllerBase } from './controller'
import maa from './maa'
import { ResourceBase } from './resource'
import { TaskDecl } from './task'

export class TaskInfo {
    instance: InstanceBase
    id: maa.TaskId

    constructor(instance: InstanceBase, id: maa.TaskId) {
        this.instance = instance
        this.id = id
    }

    set param(param: TaskDecl) {
        if (!maa.set_task_param(this.instance.handle, this.id, JSON.stringify(param))) {
            throw 'TaskInfo set param failed'
        }
    }

    wait() {
        return maa.wait_task(this.instance.handle, this.id)
    }
}

export class InstanceBase {
    handle: maa.InstanceHandle

    constructor(handle: maa.InstanceHandle) {
        this.handle = handle
    }

    notify(msg: string, detail: string): maa.MaybePromise<void> {}

    bind(slave: ControllerBase | ResourceBase) {
        let ret: boolean
        if (slave instanceof ControllerBase) {
            ret = maa.bind_controller(this.handle, slave.handle)
        } else {
            ret = maa.bind_resource(this.handle, slave.handle)
        }
        if (!ret) {
            throw 'Instance bind failed'
        }
    }

    post(type: 'task' | 'recognition' | 'action', entry: string, param: TaskDecl = {}) {
        switch (type) {
            case 'task':
                return new TaskInfo(this, maa.post_task(this.handle, entry, JSON.stringify(param)))
            case 'recognition':
                return new TaskInfo(
                    this,
                    maa.post_recognition(this.handle, entry, JSON.stringify(param))
                )
            case 'action':
                return new TaskInfo(
                    this,
                    maa.post_action(this.handle, entry, JSON.stringify(param))
                )
        }
    }

    get inited() {
        return maa.inited(this.handle)
    }

    get running() {
        return maa.running(this.handle)
    }

    post_stop() {
        if (!maa.post_stop(this.handle)) {
            throw 'Instance post_stop failed'
        }
    }
}

export class Instance extends InstanceBase {
    constructor() {
        let ws: WeakRef<this>
        const h = maa.create((msg, detail) => {
            ws.deref()?.notify(msg, detail)
        })
        if (!h) {
            throw 'Instance create failed'
        }
        super(h)
        ws = new WeakRef(this)
    }
}
