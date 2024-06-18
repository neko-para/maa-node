const maa = require('@nekosu/maa')
const whyIsNodeRunning = require('why-is-node-running')

console.log(maa.version())

maa.post_find_device()
maa.wait_for_find_device_to_complete((err, res) => {
    console.log(err, res)
    if (res) {
        const info = maa.get_device(0)
        let ctrl = maa.adb_controller_create(
            info.adb_path,
            info.adb_serial,
            info.adb_controller_type,
            info.adb_config,
            './maa/share/MaaAgentBinary',
            (msg, details) => {
                console.log(msg, details)
                return new Promise(resolve => {
                    setTimeout(() => {
                        resolve()
                    }, 10)
                })
            }
        )
        const id = maa.controller_post_connection(ctrl)
        maa.controller_wait(ctrl, id, (err, res) => {
            console.log(err, res)
        })
    }
})

// setInterval(() => {
//     whyIsNodeRunning()
// }, 2000)
