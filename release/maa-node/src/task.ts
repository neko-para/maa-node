import type { FlatRect } from './maa'

export type RGBColor = [r: number, g: number, b: number]
export type HSVColor = [h: number, s: number, v: number]
export type GrayColor = [g: number]

export type RecoType =
    | 'DirectHit'
    | 'TemplateMatch'
    | 'FeatureMatch'
    | 'ColorMatch'
    | 'OCR'
    | 'NeuralNetworkClassify'
    | 'NeuralNetworkDetect'
    | 'Custom'

export type RecoDecl =
    | {
          /**
           * 识别算法类型。可选，默认 `DirectHit`。
           *
           * 可选的值：`DirectHit` | `TemplateMatch` | `FeatureMatch` | `ColorMatch` | `OCR` | `NeuralNetworkClassify` | `NeuralNetworkDetect` | `Custom`
           *
           * - `DirectHit`: 直接命中，即不进行识别，直接执行动作。
           * - `TemplateMatch`: 模板匹配，即“找图”。
           * - `FeatureMatch`: 特征匹配，泛化能力更强的“找图”，具有抗透视、抗尺寸变化等特点。
           * - `ColorMatch`: 颜色匹配，即“找色”。
           * - `OCR`: 文字识别。
           * - `NeuralNetworkClassify`: 深度学习分类，判断图像中的 **固定位置** 是否为预期的“类别”。
           * - `NeuralNetworkDetect`: 深度学习目标检测，高级版“找图”。与分类器主要区别在于“找”，即支持任意位置。但通常来说模型复杂度会更高，需要更多的训练集、训练时间，使用时的资源占用（推理开销）也会成倍上涨。
           * - `Custom`: 执行通过 `MaaRegisterCustomRecognizer` 接口传入的识别器句柄
           *
           * @default "DirectHit"
           */
          recognition?: 'DirectHit'
      }
    | {
          recognition: 'TemplateMatch'

          /**
           * 识别区域坐标。可选，默认 [0, 0, 0, 0]，即全屏。
           *
           * 四个值分别为 [x, y, w, h]。
           *
           * @default [0, 0, 0, 0]
           */
          roi?: FlatRect | FlatRect[]

          /**
           * 模板图片路径，需要 `image` 文件夹的相对路径。必选。
           *
           * 请注意所使用的图片需要是无损原图的裁剪。若使用安卓模拟器，请使用模拟器自带的截图功能！（不可以直接对模拟器窗口进行截图）
           */
          template: string | string[]

          /**
           * 模板匹配阈值。可选，默认 0.7 。
           *
           * 若为数组，长度需和 `template` 数组长度相同。
           *
           * @default 0.7
           */
          threshold?: number | number[]

          /**
           * 结果排序方式。可选，默认 `Horizontal`。
           *
           * 可选的值：`Horizontal` | `Vertical` | `Score` | `Random`。
           *
           * 可结合 `index` 字段使用。
           *
           * @default "Horizontal"
           */
          order_by?: 'Horizontal' | 'Vertical' | 'Score' | 'Random'

          /**
           * 命中第几个结果。可选，默认 0。
           *
           * 假设共有 N 个结果，则 `index` 的取值范围为 [-N, N - 1]，其中负数使用类 Python 的规则转换为 N - index。若超出范围，则视为当前识别无结果。
           *
           * @default 0
           */
          index?: number

          /**
           * 模板匹配算法，即 cv::TemplateMatchModes。可选，默认 5 。
           *
           * 仅支持 1、3、5，可简单理解为越大的越精确，但也会更慢。
           *
           * 详情请参考 [OpenCV 官方文档](https://docs.opencv.org/4.x/df/dfb/group__imgproc__object.html)。
           *
           * @default 5
           */
          method?: 1 | 3 | 5

          /**
           * 是否进行绿色掩码。可选，默认 false。
           *
           * 若为 true，可以将图片中不希望匹配的部分涂绿 RGB: (0, 255, 0)，则不对绿色部分进行匹配。
           *
           * @default false
           */
          green_mask?: boolean
      }
    | {
          recognition: 'FeatureMatch'

          /**
           * 识别区域坐标。可选，默认 [0, 0, 0, 0]，即全屏。
           *
           * 四个值分别为 [x, y, w, h]。
           *
           * @default [0, 0, 0, 0]
           */
          roi?: FlatRect | FlatRect[]

          /**
           * 模板图片路径，需要 `image` 文件夹的相对路径。必选。
           */
          template: string | string[]

          /**
           * 匹配的特征点的数量要求（阈值），默认 4。
           *
           * @default 4
           */
          count?: number

          /**
           * 结果排序方式。可选，默认 `Horizontal`
           *
           * 可选的值：`Horizontal` | `Vertical` | `Score` | `Area` | `Random`
           *
           * 可结合 `index` 字段使用。
           *
           * @default "Horizontal"
           */
          order_by?: 'Horizontal' | 'Vertical' | 'Score' | 'Area' | 'Random'

          /**
           * 命中第几个结果。可选，默认 0。
           *
           * 假设共有 N 个结果，则 `index` 的取值范围为 [-N, N - 1]，其中负数使用类 Python 的规则转换为 N - index。若超出范围，则视为当前识别无结果。
           *
           * @default 0
           */
          index?: number

          /**
           * 是否进行绿色掩码。可选，默认 false。
           *
           * 若为 true，可以将图片中不希望匹配的部分涂绿 RGB: (0, 255, 0)，则不对绿色部分进行匹配。
           *
           * @default false
           */
          green_mask?: boolean

          /**
           * 特征检测器。可选，默认 `SIFT`。
           *
           * 目前支持以下算法：
           *
           * - SIFT
           *   计算复杂度高，具有尺度不变性、旋转不变性。效果最好。
           * - KAZE
           *   适用于2D和3D图像，具有尺度不变性、旋转不变性。
           * - AKAZE
           *   计算速度较快，具有尺度不变性、旋转不变性。
           * - BRISK
           *   计算速度非常快，具有尺度不变性、旋转不变性。
           * - ORB
           *   计算速度非常快，具有旋转不变性。但不具有尺度不变性。
           *
           * @default "SIFT"
           */
          detector?: 'SIFT' | 'KAZE' | 'AKAZE' | 'BRISK' | 'ORB'

          /**
           * KNN 匹配算法的距离比值，[0 - 1.0], 越大则匹配越宽松（更容易连线）。可选，默认 0.6。
           *
           * @default 0.6
           */
          ratio?: number
      }
    | ({
          recognition: 'ColorMatch'

          /**
           * 识别区域坐标。可选，默认 [0, 0, 0, 0]，即全屏。
           *
           * 四个值分别为 [x, y, w, h]。
           *
           * @default [0, 0, 0, 0]
           */
          roi?: FlatRect | FlatRect[]

          /**
           * 符合的点的数量要求（阈值）。可选，默认 1。
           *
           * @default 1
           */
          count?: number

          /**
           * 结果排序方式。可选，默认 `Horizontal`
           *
           * 可选的值：`Horizontal` | `Vertical` | `Score` | `Area` | `Random`
           *
           * 可结合 `index` 字段使用。
           *
           * @default "Horizontal"
           */
          order_by?: 'Horizontal' | 'Vertical' | 'Score' | 'Area' | 'Random'

          /**
           * 命中第几个结果。可选，默认 0。
           *
           * 假设共有 N 个结果，则 `index` 的取值范围为 [-N, N - 1]，其中负数使用类 Python 的规则转换为 N - index。若超出范围，则视为当前识别无结果。
           *
           * @default 0
           */
          index?: number

          /**
           * 是否是相连的点才会被计数。可选，默认否。
           *
           * 若为是，在完成颜色过滤后，则只会计数像素点 **全部相连** 的最大块。
           *
           * 若为否，则不考虑这些像素点是否相连。
           *
           * @default false
           */
          connected?: boolean
      } & (
          | {
                /**
                 * 颜色匹配方式。即 cv::ColorConversionCodes。可选，默认 4 (RGB)。
                 *
                 * 常用值：4 (RGB, 3 通道), 40 (HSV, 3 通道), 6 (GRAY, 1 通道)。
                 *
                 * 详情请参考 [OpenCV 官方文档](https://docs.opencv.org/4.x/d8/d01/group__imgproc__color__conversions.html)。
                 *
                 * @default 4
                 */
                method?: 4

                /**
                 * 颜色下限值。必选。最内层 list 长度需和 `method` 的通道数一致。
                 */
                lower: RGBColor | RGBColor[]

                /**
                 * 颜色上限值。必选。最内层 list 长度需和 `method` 的通道数一致。
                 */
                upper: RGBColor | RGBColor[]
            }
          | {
                /**
                 * 颜色匹配方式。即 cv::ColorConversionCodes。可选，默认 4 (RGB)。
                 *
                 * 常用值：4 (RGB, 3 通道), 40 (HSV, 3 通道), 6 (GRAY, 1 通道)。
                 *
                 * 详情请参考 [OpenCV 官方文档](https://docs.opencv.org/4.x/d8/d01/group__imgproc__color__conversions.html)。
                 *
                 * @default 4
                 */
                method: 40

                /**
                 * 颜色下限值。必选。最内层 list 长度需和 `method` 的通道数一致。
                 */
                lower: HSVColor | HSVColor[]

                /**
                 * 颜色上限值。必选。最内层 list 长度需和 `method` 的通道数一致。
                 */
                upper: HSVColor | HSVColor[]
            }
          | {
                /**
                 * 颜色匹配方式。即 cv::ColorConversionCodes。可选，默认 4 (RGB)。
                 *
                 * 常用值：4 (RGB, 3 通道), 40 (HSV, 3 通道), 6 (GRAY, 1 通道)。
                 *
                 * 详情请参考 [OpenCV 官方文档](https://docs.opencv.org/4.x/d8/d01/group__imgproc__color__conversions.html)。
                 *
                 * @default 4
                 */
                method: 6

                /**
                 * 颜色下限值。必选。最内层 list 长度需和 `method` 的通道数一致。
                 */
                lower: GrayColor | GrayColor[]

                /**
                 * 颜色上限值。必选。最内层 list 长度需和 `method` 的通道数一致。
                 */
                upper: GrayColor | GrayColor[]
            }
      ))
    | {
          recognition: 'OCR'

          /**
           * 识别区域坐标。可选，默认 [0, 0, 0, 0]，即全屏。
           *
           * 四个值分别为 [x, y, w, h]。
           *
           * @default [0, 0, 0, 0]
           */
          roi?: FlatRect | FlatRect[]

          /**
           * 期望的结果，支持正则。必选。
           */
          expected: string | string[]

          /**
           * 部分文字识别结果不准确，进行替换。可选。
           */
          replace?: [string, string] | [string, string][]

          /**
           * 结果排序方式。可选，默认 `Horizontal`
           *
           * 可选的值：`Horizontal` | `Vertical` | `Area` | `Length` | `Random`
           *
           * 可结合 `index` 字段使用。
           *
           * @default "Horizontal"
           */
          order_by?: 'Horizontal' | 'Vertical' | 'Area' | 'Length' | 'Random'

          /**
           * 命中第几个结果。可选，默认 0。
           *
           * 假设共有 N 个结果，则 `index` 的取值范围为 [-N, N - 1]，其中负数使用类 Python 的规则转换为 N - index。若超出范围，则视为当前识别无结果。
           *
           * @default 0
           */
          index?: number

          /**
           * 是否仅识别（不进行检测，需要精确设置 `roi`）。可选，默认 false。
           *
           * @default false
           */
          only_rec?: boolean

          /**
           * 模型 **文件夹** 路径。使用 `model/ocr` 文件夹的相对路径。可选，默认为空。
           *
           * 若为空，则为 `model/ocr` 根目录下的模型文件。
           *
           * 文件夹中需要包含 `rec.onnx`, `det.onnx`, `keys.txt` 三个文件。
           */
          model?: string
      }
    | {
          recognition: 'NeuralNetworkClassify'

          /**
           * 识别区域坐标。可选，默认 [0, 0, 0, 0]，即全屏。
           *
           * 四个值分别为 [x, y, w, h]。
           *
           * @default [0, 0, 0, 0]
           */
          roi?: FlatRect | FlatRect[]

          /**
           * 总分类数，必选。
           */
          cls_size: number

          /**
           * 标注，即每个分类的名字。可选。
           *
           * 仅影响调试图片及日志等，若未填写则会填充 "Unknown"。
           */
          labels?: string[]

          /**
           * 模型文件路径。使用 `model/classify` 文件夹的相对路径。必选。
           *
           * 目前仅支持 ONNX 模型。
           */
          model: string

          /**
           * 期望的分类下标。
           */
          expected: number | number[]

          /**
           * 结果排序方式。可选，默认 `Horizontal`
           *
           * 可选的值：`Horizontal` | `Vertical` | `Score` | `Random`
           *
           * 可结合 `index` 字段使用。
           *
           * @default "Horizontal"
           */
          order_by?: 'Horizontal' | 'Vertical' | 'Score' | 'Random'

          /**
           * 命中第几个结果。可选，默认 0。
           *
           * 假设共有 N 个结果，则 `index` 的取值范围为 [-N, N - 1]，其中负数使用类 Python 的规则转换为 N - index。若超出范围，则视为当前识别无结果。
           *
           * @default 0
           */
          index?: number
      }
    | {
          recognition: 'NeuralNetworkDetect'

          /**
           * 识别区域坐标。可选，默认 [0, 0, 0, 0]，即全屏。
           *
           * 四个值分别为 [x, y, w, h]。
           *
           * @default [0, 0, 0, 0]
           */
          roi?: FlatRect | FlatRect[]

          /**
           * 总分类数，必选。
           */
          cls_size: number

          /**
           * 标注，即每个分类的名字。可选。
           *
           * 仅影响调试图片及日志等，若未填写则会填充 "Unknown"。
           */
          labels?: string[]

          /**
           * 模型文件路径。使用 `model/classify` 文件夹的相对路径。必选。
           *
           * 目前仅支持 ONNX 模型。
           */
          model: string

          /**
           * 期望的分类下标。
           */
          expected: number | number[]

          /**
           * 模型置信度阈值。可选，默认 0.3 。
           *
           * 若为数组，长度需和 `expected` 数组长度相同。
           *
           * @default 0.3
           */
          threshold?: number

          /**
           * 结果排序方式。可选，默认 `Horizontal`
           *
           * 可选的值：`Horizontal` | `Vertical` | `Score` | `Area` | `Random`
           *
           * 可结合 `index` 字段使用。
           */
          order_by?: 'Horizontal' | 'Vertical' | 'Score' | 'Area' | 'Random'

          /**
           * 命中第几个结果。可选，默认 0。
           *
           * 假设共有 N 个结果，则 `index` 的取值范围为 [-N, N - 1]，其中负数使用类 Python 的规则转换为 N - index。若超出范围，则视为当前识别无结果。
           *
           * @default 0
           */
          index?: number
      }
    | {
          recognition: 'Custom'

          /**
           * 任务名，同 `MaaRegisterCustomRecognizer` 接口传入的识别器名。必选。
           */
          custom_recognition: string

          /**
           * 任务参数，任意类型，会在执行时透传。可选，默认空 json，即 `{}`
           */
          custom_recognition_param?: unknown
      }

export type TargetDecl = true | string | FlatRect

export type ActionType =
    | 'DoNothing'
    | 'Click'
    | 'Swipe'
    | 'Key'
    | 'Text'
    | 'StartApp'
    | 'StopApp'
    | 'StopTask'
    | 'Custom'

export type ActionDecl =
    | {
          /**
           * 执行的动作。可选，默认 `DoNothing`。
           *
           * 可选的值：`DoNothing` | `Click` | `Swipe` | `Key` | `InputText` | `StartApp` | `StopApp` | `StopTask` | `Custom`
           *
           * - `DoNothing`: 什么都不做。
           * - `Click`: 点击。
           * - `Swipe`: 滑动。
           * - `Key`: 按键。
           * - `InputText`: 输入文本。
           * - `StartApp`: 启动 App。
           * - `StopApp`: 关闭 App。
           * - `StopTask`: 停止当前任务链（MaaPostTask 传入的单个任务链）
           * - `Custom`: 执行通过 `MaaRegisterCustomAction` 接口传入的动作句柄
           *
           * @default "DoNothing"
           */
          action?: 'DoNothing'
      }
    | {
          action: 'Click'

          /**
           * 点击的位置。可选，默认 true。
           * - *true*: 点击本任务中刚刚识别到的目标（即点击自身）。
           * - *string*: 填写任务名，点击之前执行过的某任务识别到的目标。
           * - *array<int, 4>*: 点击固定坐标区域内随机一点，[x, y, w, h]，若希望全屏可设为 [0, 0, 0, 0]。
           *
           * @default true
           */
          target?: TargetDecl

          /**
           * 在 `target` 的基础上额外移动再点击，四个值分别相加。可选，默认 [0, 0, 0, 0]。
           *
           * @default [0, 0, 0, 0]
           */
          target_offset?: FlatRect
      }
    | {
          action: 'Swipe'

          /**
           * 滑动起点。可选，默认 true。
           * - *true*: 点击本任务中刚刚识别到的目标（即点击自身）。
           * - *string*: 填写任务名，点击之前执行过的某任务识别到的目标。
           * - *array<int, 4>*: 点击固定坐标区域内随机一点，[x, y, w, h]，若希望全屏可设为 [0, 0, 0, 0]。
           *
           * @default true
           */
          begin?: TargetDecl

          /**
           * 在 `begin` 的基础上额外移动再作为起点，四个值分别相加。可选，默认 [0, 0, 0, 0]。
           *
           * @default [0, 0, 0, 0]
           */
          begin_offset?: FlatRect

          /**
           * 滑动终点。必选。
           * - *true*: 点击本任务中刚刚识别到的目标（即点击自身）。
           * - *string*: 填写任务名，点击之前执行过的某任务识别到的目标。
           * - *array<int, 4>*: 点击固定坐标区域内随机一点，[x, y, w, h]，若希望全屏可设为 [0, 0, 0, 0]。
           *
           * @default true
           */
          end: TargetDecl

          /**
           * 在 `end` 的基础上额外移动再作为终点，四个值分别相加。可选，默认 [0, 0, 0, 0]。
           *
           * @default [0, 0, 0, 0]
           */
          end_offset?: FlatRect

          /**
           * 滑动持续时间，单位毫秒。可选，默认 200
           *
           * @default 200
           */
          duration?: number
      }
    | {
          action: 'Key'

          /**
           * 要按的键，仅支持对应控制器的虚拟按键码。
           * - [Adb 控制器](https://developer.android.com/reference/android/view/KeyEvent)
           * - [Win32 控制器](https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes)
           */
          key: number | number[]
      }
    | {
          action: 'InputText'

          /**
           * 要输入的文本，部分控制器仅支持 ascii。
           */
          input_text: string
      }
    | {
          action: 'StartApp'

          /**
           * 启动入口。可选，默认空。
           *
           * 需要填入 activity，例如 `com.hypergryph.arknights/com.u8.sdk.U8UnityContext`
           *
           * 若为空，将启动 `MaaControllerSetOption` - `MaaCtrlOption_DefaultAppPackageEntry` 设置的入口。
           *
           * @default ""
           */
          package?: string
      }
    | {
          action: 'StopApp'

          /**
           * 关闭要关闭的程序。可选，默认空。
           *
           * 需要填入 package name，例如 `com.hypergryph.arknights`
           *
           * 若为空，将关闭 `MaaControllerSetOption` - `MaaCtrlOption_DefaultAppPackage` 设置的 APP。
           *
           * @default ""
           */
          package?: string
      }
    | {
          action: 'StopTask'
      }
    | {
          action: 'Custom'

          /**
           * 任务名，同 `MaaRegisterCustomAction` 接口传入的动作名。必选。
           */
          custom_action: string

          /**
           * 任务参数，任意类型，通过 MaaCustomRecognizerAPI 传入 json string。可选，默认空 json，即 `{}`
           */
          custom_action_param?: unknown
      }

/**
 * 等待画面静止。需连续一定时间 画面 **没有较大变化** 才会退出动作。
 */
export type FreezeDecl =
    | number
    | {
          /**
           * 连续 `time` 毫秒 画面 **没有较大变化** 才会退出动作。可选，默认 1。
           *
           * @default 1
           */
          time?: number

          /**
           * 等待的目标。可选，默认 true。
           * - *true*: 点击本任务中刚刚识别到的目标（即点击自身）。
           * - *string*: 填写任务名，点击之前执行过的某任务识别到的目标。
           * - *array<int, 4>*: 点击固定坐标区域内随机一点，[x, y, w, h]，若希望全屏可设为 [0, 0, 0, 0]。
           *
           * @default true
           */
          target?: TargetDecl

          /**
           * 在 `target` 的基础上额外移动再作为等待目标，四个值分别相加。可选，默认 [0, 0, 0, 0]。
           *
           * @default [0, 0, 0, 0]
           */
          target_offset?: FlatRect

          /**
           * 判断“没有较大变化”的模板匹配阈值。可选，默认 0.95 。
           *
           * @default 0.95
           */
          threshold?: number

          /**
           * 判断“没有较大变化”的模板匹配算法，即 cv::TemplateMatchModes。可选，默认 5 。
           *
           * 仅支持 1、3、5，可简单理解为越大的越精确，但也会更慢。
           *
           * 详情请参考 [OpenCV 官方文档](https://docs.opencv.org/4.x/df/dfb/group__imgproc__object.html)。
           *
           * @default 5
           */
          method?: 1 | 3 | 5
      }

export type TaskRestDecl = {
    /**
     * 接下来要执行的任务列表。可选，默认空。
     *
     * 按序识别每个任务，只执行第一个识别到的。
     *
     * @default []
     */
    next?: string | string[]

    /**
     * 是否是子任务。可选，默认否。
     *
     * 如果是子任务，执行完本任务（及后续 next 等）后，会返回来再次识别本任务 **所在的** next 列表。
     *
     * 例如：A.next = [B, Sub_C, D]，这里的 Sub_C.is_sub = true，
     *
     * 若匹配上了 Sub_C，在完整执行完 Sub_C 及后续任务后，会返回来再次识别 [B, Sub_C, D] 并执行命中项及后续任务。
     *
     * @default false
     */
    is_sub?: boolean

    /**
     * 反转识别结果，识别到了当做没识别到，没识别到的当做识别到了。可选，默认否。
     *
     * 请注意由此识别出的任务，Click 等动作的点击自身将失效（因为实际并没有识别到东西），若有需求可单独设置 `target`。
     *
     * @default false
     */
    inverse?: boolean

    /**
     * 是否启用该 task。可选，默认 true。
     *
     * 若为 false，则其他 task 的 next 列表中的该 task，会被跳过，既不会被识别也不会被执行。
     *
     * @default true
     */
    enabled?: boolean

    /**
     * `next` 识别超时时间，毫秒。默认 20 * 1000。
     *
     * @default 20000
     */
    timeout?: number

    /**
     * 超时后执行的任务列表。可选，默认空。
     *
     * @default []
     */
    timeout_next?: string | string[]

    /**
     * 任务执行次数。可选，默认 UINT_MAX。
     *
     * @default 4294967295
     */
    times_limit?: number

    /**
     * 任务执行次数达到了后执行的任务列表，可选，默认空。
     *
     * @default []
     */
    runout_next?: string | string[]

    /**
     * 识别到 到 执行动作前 的延迟，毫秒。可选，默认 200。
     *
     * 推荐尽可能增加中间过程任务，少用延迟，不然既慢还不稳定。
     *
     * @default 200
     */
    pre_delay?: number

    /**
     * 执行动作后 到 识别 next 的延迟，毫秒。可选，默认 500。
     *
     * 推荐尽可能增加中间过程任务，少用延迟，不然既慢还不稳定。
     *
     * @default 500
     */
    post_delay?: number

    /**
     * 识别到 到 执行动作前，等待画面不动了的时间，毫秒。可选，默认 0，即不等待。
     *
     * 连续 `pre_wait_freezes` 毫秒 画面 **没有较大变化** 才会退出动作。
     *
     * 若为 object，可设置更多参数。
     *
     * 具体的顺序为 `pre_wait_freezes` - `pre_delay` - `action` - `post_wait_freezes` - `post_delay`。
     *
     * @default 0
     */
    pre_wait_freezes?: FreezeDecl

    /**
     * 行动动作后 到 识别 next，等待画面不动了的时间，毫秒。可选，默认 0，即不等待。
     *
     * 连续 `pre_wait_freezes` 毫秒 画面 **没有较大变化** 才会退出动作。
     *
     * 若为 object，可设置更多参数。
     *
     * 具体的顺序为 `pre_wait_freezes` - `pre_delay` - `action` - `post_wait_freezes` - `post_delay`。
     *
     * @default 0
     */
    prost_wait_freezes?: FreezeDecl

    /**
     * 是否关注任务，会额外产生部分回调消息。可选，默认 false，即不产生。
     *
     * @default false
     */
    focus?: boolean
}

export type TaskDecl = RecoDecl & ActionDecl & TaskRestDecl

export type PipelineDecl = {
    [task: string]: TaskDecl
}
