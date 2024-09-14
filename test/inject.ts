import { existsSync } from 'fs'

declare module globalThis {
    let MaaAPI: any
}

for (const p of ['../build/maa.node', '../build/Release/maa.node']) {
    if (existsSync(p)) {
        globalThis.MaaAPI = require(p)
        break
    }
}
