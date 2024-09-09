import * as maa from './maa'

export interface JobSource<Id> {
    status(id: Id): maa.Status
    wait(id: Id): Promise<maa.Status>
}

export class Job<Id, Source extends JobSource<Id>> {
    source: Source
    id: Id

    constructor(source: Source, id: Id) {
        this.source = source
        this.id = id
    }

    get status(): maa.Status {
        return this.source.status(this.id)
    }

    async wait(): Promise<this> {
        await this.source.wait(this.id)
        return this
    }

    get done() {
        return [maa.Status.Success, maa.Status.Failed].includes(this.status)
    }

    get success() {
        return this.status == maa.Status.Success
    }

    get failed() {
        return this.status == maa.Status.Failed
    }

    get running() {
        return this.status == maa.Status.Running
    }

    get pending() {
        return this.status == maa.Status.Pending
    }
}
