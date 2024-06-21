import { ControllerBase } from './controller'
import { ImageBuffer, ImageListBuffer } from './image'
import maa from './maa'
import { ResourceBase } from './resource'
import { SyncContext } from './syncContext'
import { TaskDecl } from './task'

export type CustomRecognizer = (
    sync_context: SyncContext,
    name: string,
    param: string,
    image: ImageBuffer
) => maa.MaybePromise<{
    box: maa.Rect
    detail: string
}>

export type CustomAction = (
    sync_context: SyncContext,
    name: string,
    param: string,
    box: maa.Rect,
    rec_detail: string
) => maa.MaybePromise<boolean>

export class RecoInfo {
    id: maa.RecoId

    constructor(id: maa.RecoId) {
        this.id = id
    }

    detail(raw?: ImageBuffer, draws?: ImageListBuffer) {
        return maa.query_recognition_detail(this.id, raw?.handle ?? null, draws?.handle ?? null)
    }
}

export class NodeInfo {
    id: maa.NodeId

    constructor(id: maa.NodeId) {
        this.id = id
    }

    get detail() {
        const info = maa.query_node_detail(this.id)
        return info
            ? {
                  name: info.name,
                  recos: new RecoInfo(info.reco_id),
                  run_completed: info.run_completed
              }
            : null
    }
}

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

    get detail() {
        const info = maa.query_task_detail(this.id)
        return info
            ? {
                  entry: info.entry,
                  nodes: info.node_id_list.map(id => new NodeInfo(id))
              }
            : null
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

    register_custom_recognizer(name: string, reco: CustomRecognizer) {
        if (
            !maa.register_custom_recognizer(this.handle, name, async (ctx, image, name, param) => {
                const result = await reco(new SyncContext(ctx), name, param, new ImageBuffer(image))
                if (result) {
                    return {
                        out_box: result.box,
                        out_detail: result.detail
                    }
                } else {
                    return null
                }
            })
        ) {
            throw 'Instance register_custom_recognizer failed'
        }
    }

    unregister_custom_recognizer(name?: string) {
        let ret: boolean
        if (name) {
            ret = maa.unregister_custom_recognizer(this.handle, name)
        } else {
            ret = maa.clear_custom_recognizer(this.handle)
        }
        if (!ret) {
            throw 'Instance unregister_custom_recognizer failed'
        }
    }

    register_custom_action(name: string, action: CustomAction) {
        if (
            !maa.register_custom_action(this.handle, name, (ctx, ...args) =>
                action(new SyncContext(ctx), ...args)
            )
        ) {
            throw 'Instance register_custom_action failed'
        }
    }

    unregister_custom_action(name?: string) {
        let ret: boolean
        if (name) {
            ret = maa.unregister_custom_action(this.handle, name)
        } else {
            ret = maa.clear_custom_action(this.handle)
        }
        if (!ret) {
            throw 'Instance unregister_custom_action failed'
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
