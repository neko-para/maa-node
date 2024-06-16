const maa = require('./build/maa.node')

console.log(maa.version())

const handle = maa.create_image_buffer()

console.log(handle)

console.log(maa.is_image_empty(handle))
