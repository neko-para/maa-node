import * as maa from './maa'

console.log(maa.Global.version)

maa.Global.log_dir = 'debug'

async function main() {
    const devices = await maa.AdbController.find()

    if (!devices || devices.length === 0) {
        return
    }

    const [name, adb_path, address, screencap_methods, input_methods, config] = devices[0]

    const ctrl = new maa.AdbController(
        adb_path,
        address,
        (screencap_methods as bigint) - BigInt(maa.api.AdbScreencapMethod.RawByNetcat),
        input_methods,
        config
    )
    ctrl.notify = (msg, detail) => {
        console.log(msg, detail)
    }
    await ctrl.post_connection().wait()
    const res = new maa.Resource()
    res.notify = (msg, detail) => {
        console.log(msg, detail)
    }
    const tskr = new maa.Tasker()
    tskr.notify = (msg, detail) => {
        console.log(msg, detail)
    }
    tskr.bind(ctrl)
    tskr.bind(res)

    console.log(tskr.inited)

    res.register_custom_recognizer('direct', function () {
        return [
            {
                x: 0,
                y: 0,
                width: 0,
                height: 0
            },
            '111'
        ]
    })

    res.register_custom_action('print', function () {
        console.log(this.param)
        return true
    })

    if (
        (
            await tskr
                .post_pipeline('testCustom', {
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
                .wait()
        ).success
    ) {
        console.log('success!')
    }

    tskr.destroy()
    ctrl.destroy()
    res.destroy()
}

main()
