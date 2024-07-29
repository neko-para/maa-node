#!/usr/bin/env node
const fs = require('fs')
const child_process = require('child_process')
const util = require('util')

async function main() {
    if (fs.existsSync('MaaCommonAssets')) {
        console.log('pulling MaaCommonAssets')
        await util.promisify(child_process.exec)('git -C MaaCommonAssets pull')
    } else {
        console.log('downloading MaaCommonAssets')
        await util.promisify(child_process.exec)(
            'git clone https://github.com/MaaXYZ/MaaCommonAssets/ --depth=1'
        )
    }
    fs.mkdirSync('resource/model', { recursive: true })
    fs.mkdirSync('resource/image', { recursive: true })
    fs.mkdirSync('resource/pipeline', { recursive: true })
    fs.cpSync('MaaCommonAssets/OCR/ppocr_v4/zh_cn', 'resource/model/ocr', { recursive: true })
    fs.writeFileSync(
        'resource/pipeline/task.json',
        JSON.stringify({
            Task1: {}
        })
    )
    fs.rm('MaaCommonAssets', { recursive: true }, err => {
        console.log(err)
    })
}

main()
