# 自定义识别/操作

* 注意, 后面注册的任务的返回结果均可以是对应类型的Promise, 即你可以在其中进行异步操作. 所有通过Context执行的操作均是异步的.

## 自定义识别

```json
{
    "Task": {
        "recognition": "Custom",
        "custom_recognition": "myReco",
        "custom_recognition_param": {
            "msg": "Hello world!"
        },
    }
}
```

```javascript
export interface CustomRecognizerSelf {
    context: Context
    id: maa.TaskId
    name: string
    param: unknown
    image: maa.ImageData
}

res.register_custom_recognizer('myReco', function (this: CustomRecognizerSelf, self: CustomRecognizerSelf) {
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
```

* `this`和第一个参数`self`是同一个`CustomRecognizerSelf`对象, 包含当次执行的各种信息.
* `this.context`是一个`Context`对象, 通过它可以获取执行其它任务, 或获取`Tasker`, `Controller`以进行点击等操作.
* `this.image`是一个`ArrayBuffer`对象, 根据这个图片进行实际的识别操作, 或是调用其它识别任务进行识别.
* `this.param`是一个值, 来自`pipeline`中提供的`custom_recognition_param`, 即`{ "msg": "Hello world!" }`. 已经被`JSON.parse`.
* 识别命中则返回一个数组, 其中包含识别结果和识别信息, 未命中则返回`null`.

下面是一个调用其它任务进行识别的例子

```javascript
inst.register_custom_recognizer('forward', (self) => {
    return self.context.run_recognition('another_task', self.image)
})
```

## 自定义任务

```json
{
    "Task": {
        "action": "Custom",
        "custom_action": "myAct",
        "custom_action_param": {
            "msg": "Hello world!"
        },
    }
}
```

```javascript
interface CustomActionSelf {
    context: Context
    id: maa.TaskId
    name: string
    param: unknown
    box: maa.Rect
    detail: string
}

inst.register_custom_action('myAct', function (this: CustomActionSelf, self: CustomActionSelf) => {
    return true
})
```

* `this`和第一个参数`self`是同一个`CustomActionSelf`对象, 包含当次执行的各种信息.
* `this.context`是一个`Context`对象, 通过它可以获取执行其它任务, 或获取`Tasker`, `Controller`以进行点击等操作.
* `this.param`是一个值, 来自`pipeline`中提供的`custom_action_param`, 即`{ "msg": "Hello world!" }`
* `this.box`是一个`Rect`对象, 即识别任务中的识别范围.
* `this.detail`是一个字符串, 即识别任务中的识别信息.
* 任务完成则返回`true`, 否则返回`false`.

下面是一个调用其它任务进行操作的例子

```javascript
inst.register_custom_action('forward', (self) => {
    return self.context.run_action('another_task', box, detail)
})
```
