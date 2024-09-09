declare namespace globalThis {
    let MaaAPI: any
}

globalThis.MaaAPI = require('../build/maa.node')
