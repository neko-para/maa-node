declare module globalThis {
    let MaaAPI: any
}

if (process.platform === 'win32') {
    globalThis.MaaAPI = require('../build/maa.node')
} else {
    globalThis.MaaAPI = require('../build/Release/maa.node')
}
