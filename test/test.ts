import * as maa from './maa'

console.log(maa.version())

async function main() {
    const devices = await maa.AdbController.find()

    if (!devices) {
        return
    }

    const ctrl = new maa.AdbController(devices[0])
    ctrl.notify = (msg, detail) => {
        console.log(msg, detail)
    }
    await ctrl.post_connection()
    const res = new maa.Resource()
    res.notify = (msg, detail) => {
        console.log(msg, detail)
    }
    const inst = new maa.Instance()
    inst.notify = (msg, detail) => {
        console.log(msg, detail)
    }
    inst.bind(ctrl)
    inst.bind(res)

    console.log(inst.inited)

    inst.register_custom_recognizer('direct', (ctx, name, param, image) => {
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

    inst.register_custom_action('print', (ctx, name, param, box, detail) => {
        return true
    })

    await inst
        .post('task', 'testCustom', {
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
}

main()
