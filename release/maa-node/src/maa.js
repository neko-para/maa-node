switch (`${process.platform}-${process.arch}`) {
    case 'win32-x64':
        module.exports = require('@nekosu/maa-node-win32-x64')
        break
    case 'win32-arm64':
        module.exports = require('@nekosu/maa-node-win32-arm64')
        break
    case 'linux-x64':
        module.exports = require('@nekosu/maa-node-linux-x64')
        break
    case 'linux-arm64':
        module.exports = require('@nekosu/maa-node-linux-arm64')
        break
    case 'darwin-x64':
        module.exports = require('@nekosu/maa-node-darwin-x64')
        break
    case 'darwin-arm64':
        module.exports = require('@nekosu/maa-node-darwin-arm64')
        break
    default:
        module.exports = {}
        break
}
