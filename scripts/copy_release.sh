#!/bin/bash

VERSION_REF="$1"

if [[ "$VERSION_REF" == "" ]]; then
    echo "No ref specified, stop"
    exit 1;
fi

VERSION=${VERSION_REF#refs/tags/v}

if [[ "$VERSION_REF" == "$VERSION" ]]; then
    echo "Invalid ref $VERSION_REF, stop"
    exit 1;
fi

echo "Release version $VERSION"

# 定义源目录和目标目录
SOURCE_DIR="assets"
TARGET_DIR="release"

# 创建目标目录（如果不存在）
mkdir -p "$TARGET_DIR"

chmod +x -R $SOURCE_DIR

# 遍历源目录中的每个子目录
for dir in "$SOURCE_DIR"/maa-node-*; do
  if [ -d "$dir" ]; then
    # 获取子目录的名称
    dir_name=$(basename "$dir")

    # 定义目标目录路径
    target_dir="$TARGET_DIR/$dir_name"

    # 创建目标目录（如果不存在）
    mkdir -p "$target_dir"

    # 复制文件到目标目录
    cp -r "$dir"/* "$target_dir"

    npm pkg set optionalDependencies.@nekosu/${dir_name}=$VERSION --prefix "$TARGET_DIR/maa-node"
  fi
done

for dir in "$TARGET_DIR"/*; do
  npm pkg set version=$VERSION --prefix "$dir"
done

cp -r maa/share/MaaAgentBinary/* release/maa-node/agent
mkdir -p release/maa-node/dist
cp assets/maa-wrapper/* release/maa-node/dist
