if (process.platform === 'win32') {
    module.exports = require("../build/maa.node");
} else {
    module.exports = require('../build/Release/maa.node')
}
