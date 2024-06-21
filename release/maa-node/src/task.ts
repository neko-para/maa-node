import { Rect } from './maa'

type RecoDecl =
    | {
          recognition?: 'DirectHit'
      }
    | {
          recognition: 'TemplateMatch'
          roi?: Rect | Rect[]
          template: string | string[]
          threshold?: number | number[]
          order_by?: 'Horizontal' | 'Vertical' | 'Score' | 'Random'
          index?: number
          method?: 1 | 3 | 5
          green_mask?: boolean
      }
    | {
          recognition: 'FeatureMatch'
          roi?: Rect | Rect[]
          template: string | string[]
          count?: number
          threshold?: number | number[]
          order_by?: 'Horizontal' | 'Vertical' | 'Score' | 'Area' | 'Random'
          index?: number
          green_mask?: boolean
          detector?: 'SIFT' | 'KAZE' | 'AKAZE' | 'BRISK' | 'ORB'
          ratio?: number
      }
    | {
          recognition: 'ColorMatch'
          roi?: Rect | Rect[]
          method?: 4 | 40 | 6
          lower: number[] | number[][]
          upper: number[] | number[][]
          count?: number
          order_by?: 'Horizontal' | 'Vertical' | 'Score' | 'Area' | 'Random'
          index?: number
          connected?: boolean
      }
    | {
          recognition: 'OCR'
          roi?: Rect | Rect[]
          expected: string | string[]
          replace?: [string, string] | [string, string][]
          order_by?: 'Horizontal' | 'Vertical' | 'Area' | 'Length' | 'Random'
          index?: number
          only_rec?: boolean
          model?: string
      }
    | {
          recognition: 'NeuralNetworkClassify'
          roi?: Rect | Rect[]
          cls_size: number
          labels?: string[]
          model: string
          expected: number | number[]
          order_by?: 'Horizontal' | 'Vertical' | 'Score' | 'Random'
          index?: number
      }
    | {
          recognition: 'NeuralNetworkDetect'
          roi?: Rect | Rect[]
          cls_size: number
          labels?: string[]
          model: string
          expected: number | number[]
          threshold?: number
          order_by?: 'Horizontal' | 'Vertical' | 'Score' | 'Area' | 'Random'
          index?: number
      }
    | {
          recognition: 'Custom'
          custom_recognition: string
          custom_recognition_param?: unknown
      }

type TargetDecl = true | string | Rect

type ActionDecl =
    | {
          action?: 'DoNothing'
      }
    | {
          action: 'Click'
          target?: TargetDecl
          target_offset?: Rect
      }
    | {
          action: 'Swipe'
          begin?: TargetDecl
          begin_offset?: Rect
          end: TargetDecl
          end_offset?: Rect
          duration?: number
      }
    | {
          action: 'Key'
          key: number | number[]
      }
    | {
          action: 'InputText'
          input_text: string
      }
    | {
          action: 'StartApp'
          package?: string
      }
    | {
          action: 'StopApp'
          package?: string
      }
    | {
          action: 'StopTask'
      }
    | {
          action: 'Custom'
          custom_action: string
          custom_action_param?: unknown
      }

type FreezeDecl =
    | number
    | {
          time?: number
          target?: TargetDecl
          target_offset?: Rect
          threshold?: number
          method?: 1 | 3 | 5
      }

export type TaskDecl = RecoDecl &
    ActionDecl & {
        next?: string | string[]
        is_sub?: boolean
        inverse?: boolean
        enabled?: boolean
        timeout?: number
        timeout_next?: string | string[]
        times_limit?: number
        runout_next?: string | string[]
        pre_delay?: number
        post_delay?: number
        pre_wait_freezes?: FreezeDecl
        prost_wait_freezes?: FreezeDecl
        focus?: boolean
    }
