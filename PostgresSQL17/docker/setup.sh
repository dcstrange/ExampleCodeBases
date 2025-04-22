#!/bin/bash

# 创建数据目录结构
echo "解压Neo4j数据..."
tar -xzvf neo4j-data.tar.gz

# 确保权限正确
chmod -R 777 neo4j-data

echo "尝试启动Neo4j容器..."

# 方法1: 尝试使用新版Docker Compose命令格式
if docker compose version &>/dev/null; then
    echo "使用新版Docker Compose命令启动..."
    docker compose up -d
    docker compose ps
elif command -v python3 &>/dev/null; then
    # 方法2: 尝试安装distutils修复
    echo "尝试安装Python依赖..."
    if command -v apt &>/dev/null; then
        sudo apt update && sudo apt install -y python3-distutils
        docker-compose up -d
        docker-compose ps
    elif command -v dnf &>/dev/null; then
        sudo dnf install -y python3-setuptools
        docker-compose up -d
        docker-compose ps
    else
        echo "无法自动安装Python依赖，使用纯Docker命令启动..."
        # 方法3: 使用纯Docker命令
        docker run -d --name simplefs \
          -p 7674:7474 \
          -p 7689:7687 \
          -v $(pwd)/neo4j-data/postgressql17/data:/data \
          -v $(pwd)/neo4j-data/postgressql17/logs:/logs \
          -v $(pwd)/neo4j-data/postgressql17/backups:/backups \
          -v $(pwd)/neo4j-data/postgressql17/import:/var/lib/neo4j/import \
          -e NEO4J_apoc_import_file_enabled=true \
          -e NEO4J_apoc_export_file_enabled=true \
          -e NEO4J_apoc_import_file_use__neo4j__config=true \
          -e 'NEO4J_PLUGINS=["graph-data-science", "apoc"]' \
          -e NEO4J_AUTH=neo4j/passwd123 \
          -e NEO4J_dbms_default__listen__address=0.0.0.0 \
          neo4j:latest
        
        docker ps | grep simplefs
    fi
else
    # 方法3: 使用纯Docker命令
    echo "使用纯Docker命令启动..."
    docker run -d --name simplefs \
      -p 7674:7474 \
      -p 7689:7687 \
      -v $(pwd)/neo4j-data/postgressql17/data:/data \
      -v $(pwd)/neo4j-data/postgressql17/logs:/logs \
      -v $(pwd)/neo4j-data/postgressql17/backups:/backups \
      -v $(pwd)/neo4j-data/postgressql17/import:/var/lib/neo4j/import \
      -e NEO4J_apoc_import_file_enabled=true \
      -e NEO4J_apoc_export_file_enabled=true \
      -e NEO4J_apoc_import_file_use__neo4j__config=true \
      -e 'NEO4J_PLUGINS=["graph-data-science", "apoc"]' \
      -e NEO4J_AUTH=neo4j/passwd123 \
      -e NEO4J_dbms_default__listen__address=0.0.0.0 \
      neo4j:latest
    
    docker ps | grep simplefs
fi

echo "Neo4j已启动！"
echo "浏览器界面: http://localhost:7674"
echo "用户名: neo4j"
echo "密码: passwd123"