import maa from './maa'

export class ImageBuffer {
    handle: maa.ImageBufferHandle

    constructor(handle?: maa.ImageBufferHandle) {
        const h = handle ?? maa.create_image_buffer()
        if (!h) {
            throw 'ImageBuffer create failed'
        }
        this.handle = h
    }

    get empty() {
        return maa.is_image_empty(this.handle)
    }

    clear() {
        if (!maa.clear_image(this.handle)) {
            throw 'ImageBuffer clear failed'
        }
    }

    get info() {
        return maa.get_image_info(this.handle)
    }

    get encoded() {
        return maa.get_image_encoded(this.handle)
    }

    set encoded(data: ArrayBuffer | Buffer) {
        if (!maa.set_image_encoded(this.handle, data)) {
            throw 'ImageBuffer set encoded failed'
        }
    }
}

export class ImageListBuffer {
    handle: maa.ImageListBufferHandle

    constructor(handle?: maa.ImageListBufferHandle) {
        const h = handle ?? maa.create_image_list_buffer()
        if (!h) {
            throw 'ImageListBuffer create failed'
        }
        this.handle = h
    }

    get empty() {
        return maa.is_image_list_empty(this.handle)
    }

    clear() {
        if (!maa.clear_image_list(this.handle)) {
            throw 'ImageListBuffer clear failed'
        }
    }

    get size() {
        return maa.get_image_list_size(this.handle)
    }

    at(index: number) {
        return new ImageBuffer(maa.get_image_list_at(this.handle, index))
    }

    append(img: ImageBuffer) {
        if (!maa.image_list_append(this.handle, img.handle)) {
            throw 'ImageBuffer append failed'
        }
    }

    remove(index: number) {
        if (!maa.image_list_remove(this.handle, index)) {
            throw 'ImageBuffer remove failed'
        }
    }
}
