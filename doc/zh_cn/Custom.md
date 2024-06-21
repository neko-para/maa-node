# 自定义识别/操作

* 注意, 后面注册的任务的返回结果均可以是对应类型的Promise, 即你可以在其中进行异步操作. 所有通过SyncContext执行的操作均是异步的.

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
inst.register_custom_recognizer('myReco', (ctx, name, param, image) => {
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
```

* `ctx`是一个`SyncContext`对象, 通过它可以执行其它任务或进行点击等操作.
* `image`是一个`ImageBuffer`对象, 根据这个图片进行实际的识别操作, 或是调用其它识别任务进行识别
* `param`是一个值, 来自`pipeline`中提供的`custom_recognition_param`, 即`{ "msg": "Hello world!" }`
* 识别命中则返回一个对象, 其中包含识别结果`out_box`和识别信息`out_detail`, 未命中则返回`null`

下面是一个调用其它任务进行识别的例子

```javascript
inst.register_custom_recognizer('forward', (ctx, name, param, image) => {
    return ctx.run_recognition('another_task', image)
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
inst.register_custom_action('myAct', (ctx, name, param, box, detail) => {
    return true
})
```

* `ctx`是一个`SyncContext`对象, 通过它可以执行其它任务或进行点击等操作.
* `param`是一个值, 来自`pipeline`中提供的`custom_recognition_param`, 即`{ "msg": "Hello world!" }`
* `box`是一个`Rect`对象, 即识别任务中的识别范围.
* `detail`是一个字符串, 即识别任务中的识别信息.
* 任务完成则返回`true`, 否则返回`false`

下面是一个调用其它任务进行操作的例子

```javascript
inst.register_custom_action('forward', (ctx, name, param, box, detail) => {
    return ctx.run_action('another_task', box, detail)
})
```
