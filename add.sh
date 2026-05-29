#!/bin/bash
for i in {1..5};do
    (echo "add,测试商品$i,10.0,100";sleep 1;echo "quit") | ./client &
done
wait
echo "执行完毕"