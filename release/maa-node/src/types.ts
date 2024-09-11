import { Context } from './context'
import maa from './maa'

export interface CustomRecognizerSelf {
    context: Context
    id: maa.TaskId
    name: string
    param: unknown
    image: maa.ImageData
}

export interface CustomActionSelf {
    context: Context
    id: maa.TaskId
    name: string
    param: unknown
    box: maa.Rect
    detail: string
}
