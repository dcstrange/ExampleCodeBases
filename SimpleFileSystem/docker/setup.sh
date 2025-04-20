#!/bin/bash

# 解压数据（使用正确的路径）
echo "解压Neo4j数据..."
tar -xzvf neo4j-simplefs.tar.gz

# 确保权限正确
chmod -R 777 neo4j-data

# 启动容器
echo "启动Neo4j容器..."
docker-compose up -d

# 检查容器状态
echo "检查容器状态..."
docker-compose ps

echo "Neo4j已启动！"
echo "浏览器界面: http://localhost:7674"
echo "用户名: neo4j"
echo "密码: passwd123"