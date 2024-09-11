import { Context } from './context'
import * as maa from './maa'
import { CustomActionSelf, CustomRecognizerSelf } from './types'

export const Pi = {
    __running: false,

    notify(message: string, details_json: string): maa.MaybePromise<void> {},

    register_custom_recognizer(
        name: string,
        func: (
            this: CustomRecognizerSelf,
            self: CustomRecognizerSelf
        ) => maa.MaybePromise<[out_box: maa.Rect, out_detail: string] | null>
    ) {
        maa.pi_register_custom_recognizer(name, (context, id, name, param, image) => {
            const self: CustomRecognizerSelf = {
                context: new Context(context),
                id,
                name,
                param: JSON.parse(param),
                image
            }
            return func.apply(self, [self])
        })
    },

    register_custom_action(
        name: string,
        func: (this: CustomActionSelf, self: CustomActionSelf) => maa.MaybePromise<boolean>
    ) {
        maa.pi_register_custom_action(name, (context, id, name, param, box, detail) => {
            const self: CustomActionSelf = {
                context: new Context(context),
                id,
                name,
                param: JSON.parse(param),
                box,
                detail
            }
            return func.apply(self, [self])
        })
    },

    async run_cli(resource_path: string, user_path: string, directly: boolean) {
        if (Pi.__running) {
            return false
        }
        Pi.__running = true
        const res = await maa.pi_run_cli(resource_path, user_path, directly, (message, details) => {
            return Pi.notify(message, details)
        })
        Pi.__running = false
        return res
    }
}
