#!/bin/bash
# 备份product.dat到backup目录里，文件名带日期
backup_dir="$HOME/my_project/backup"
mkdir -p "$backup_dir"
date_str=$(date +%Y%m%d_%H%M%S)
cp product.dat "$backup_dir/product_$date_str.dat"
echo "备份完成：$backup_dir/product_$date_str.dat"