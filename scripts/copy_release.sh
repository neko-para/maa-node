#!/bin/bash

# 定义源目录和目标目录
SOURCE_DIR="assets"
TARGET_DIR="release"

# 创建目标目录（如果不存在）
mkdir -p "$TARGET_DIR"

# 遍历源目录中的每个子目录
for dir in "$SOURCE_DIR"/maa-node-*; do
  if [ -d "$dir" ]; then
    # 获取子目录的名称
    dir_name=$(basename "$dir")
    
    # 移除 'node-' 部分
    target_dir_name=${dir_name/node-/}
    
    # 定义目标目录路径
    target_dir="$TARGET_DIR/$target_dir_name"
    
    # 创建目标目录（如果不存在）
    mkdir -p "$target_dir"
    
    # 复制文件到目标目录
    cp -r "$dir"/* "$target_dir"
    
    echo "Copied files from $dir to $target_dir"
  fi
done
