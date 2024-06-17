const maa = require('./dist')

console.log(maa.version())

const handle = maa.create_image_buffer()

console.log(handle)

console.log(maa.is_image_empty(handle))

maa.post_find_device()
maa.wait_for_find_device_to_complete((err, res) => {
    console.log(err, res)
})
