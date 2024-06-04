#!/bin/bash

srcPath="/home/app/ota_backup"
desPath="/home/app"
AppName="ControlBox"
UpdaterName="OtaUpdater"

# 检查源文件路径是否存在
if [ -d "$srcPath" ]; then
  # 检查目标文件中的$AppName文件是否存在
  if [ -f "$desPath/$AppName" ]; then
    echo "$AppName 文件存在，杀死$AppName进程"
    pkill -f $AppName
    pkill -f $UpdaterName
  fi

  # 源文件路径存在，开始拷贝操作
  for file in "$srcPath"/*; do
    if [ -f "$file" ]; then
      # 如果是文件，则进行拷贝
      cp -f "$file" "$desPath"
    fi
  done

  # 检查拷贝是否成功
  if [ $? -eq 0 ]; then
    echo "文件拷贝成功"

    # 检查目标文件中的$AppName文件是否存在
    if [ -f "$desPath/$AppName" ]; then
      echo "$AppName 文件存在，启动$AppName"

      # 给文件权限777
      chmod 777 "$desPath/$UpdaterName"
      chmod 777 "$desPath/$AppName"
      "$desPath/$AppName" &
      "$desPath/$UpdaterName" &
    else
      echo "$AppName 文件不存在"
    fi
  else
    echo "文件拷贝失败"
  fi
else
  echo "源文件路径不存在"
fi
