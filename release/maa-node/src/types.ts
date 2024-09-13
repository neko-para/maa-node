import { Context } from './context'
import maa from './maa'

export interface CustomRecognizerSelf {
    context: Context
    id: maa.TaskId
    task: string
    name: string
    param: unknown
    image: maa.ImageData
}

export interface CustomActionSelf {
    context: Context
    id: maa.TaskId
    task: string
    name: string
    param: unknown
    recoId: maa.RecoId
    box: maa.Rect
}
