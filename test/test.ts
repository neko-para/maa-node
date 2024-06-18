import maa from './maa'
import fs from 'fs'
import util from 'util'

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
                return new Promise(resolve => {
                    setTimeout(() => {
                        resolve()
                    }, 10)
                })
            }
        )
        if (ctrl) {
            await util.promisify(maa.controller_wait)(ctrl, maa.controller_post_connection(ctrl))
            await util.promisify(maa.controller_wait)(ctrl, maa.controller_post_screencap(ctrl))
            const img = maa.create_image_buffer()!
            maa.controller_get_image(ctrl, img)
            fs.writeFileSync('result.png', Buffer.from(maa.get_image_encoded(img)))
        }
    }
}

main()
