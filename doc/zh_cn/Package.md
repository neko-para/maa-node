# 打包

本项目中包含二进制, `@nekosu/maa-node`包会从`@nekosu/maa-node-${platform}-${arch}`包中获取对应`maa.node`. 同时, 由于被加载的二进制`maa.node`动态链接了`MaaFramework`库, 且`Adb`控制器依赖`AgentBinary`, 建议直接将`@nekosu/maa-node`作为外部依赖不参与打包.

如果你确实想要打包, 需要注意一下要点:

* 通常打包工具只能发现`maa.node`, 但不会意识到它的依赖. `maa.node`旁边的所有`dll/so/dylib`应当一并放置在`maa.node`同目录中.
* `Adb`控制器会使用脚本所在目录的`../agent`目录作为MaaAgentBinary.
