switch (`${process.platform}-${process.arch}`) {
    case 'win32-x64':
        module.exports = require('@nekosu/maa-win32-x64')
        break
    case 'win32-arm64':
        module.exports = require('@nekosu/maa-win32-arm64')
        break
    case 'linux-x64':
        module.exports = require('@nekosu/maa-linux-x64')
        break
    case 'linux-arm64':
        module.exports = require('@nekosu/maa-linux-arm64')
        break
    case 'darwin-x64':
        module.exports = require('@nekosu/maa-darwin-x64')
        break
    case 'darwin-arm64':
        module.exports = require('@nekosu/maa-darwin-arm64')
        break
    default:
        module.exports = {}
        break
}
