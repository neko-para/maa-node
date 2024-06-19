import fs from 'fs'
import util from 'util'

import maa from './maa'

console.log(maa.version())

async function main() {
    maa.post_find_device()
    const res = await util.promisify(maa.wait_for_find_device_to_complete)()

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
            }
        )
        if (!ctrl) {
            return
        }
        await util.promisify(maa.controller_wait)(ctrl, maa.controller_post_connection(ctrl))
        const res = maa.resource_create((msg, details) => {
            console.log(msg, details)
        })
        const inst = maa.create((msg, details) => {
            console.log(msg, details)
        })
        if (!res || !inst) {
            return
        }
        maa.bind_resource(inst, res)
        maa.bind_controller(inst, ctrl)
        console.log(maa.inited(inst))

        maa.register_custom_recognizer(inst, 'direct', (...args) => {
            console.log(...args)
            return {
                out_box: {
                    x: 0,
                    y: 0,
                    width: 0,
                    height: 0
                },
                out_detail: '111'
            }
        })
        maa.register_custom_action(inst, 'print', (...args) => {
            console.log(...args)
            return true
        })
        const task = maa.post_task(
            inst,
            'testCustom',
            JSON.stringify({
                test: {
                    action: 'StartApp',
                    package: 'com.android.gallery3d/com.android.gallery3d.app.GalleryActivity'
                },
                testCustom: {
                    recognition: 'Custom',
                    custom_recognition: 'direct',
                    custom_recognition_param: {
                        msg: 'Hello world!'
                    },
                    action: 'Custom',
                    custom_action: 'print',
                    custom_action_param: {
                        msg: 'Hello world!'
                    }
                }
            })
        )
        await util.promisify(maa.wait_task)(inst, task)
        globalThis.gc?.()
        // process.exit()
    }
}

main()
