# Neo4j SimpleFS 分发包

## 前置条件
- 已安装Docker
- 已安装Docker Compose

## 快速部署
1. 解压此分发包
2. 执行 `sudo ./setup.sh`
3. 访问 http://localhost:7674
4. 使用以下凭据登录:
   - 用户名: neo4j
   - 密码: passwd123

## 手动安装步骤
1. 解压数据: `tar -xzvf neo4j-simplefs.tar.gz`
2. 启动容器: `sudo docker-compose up -d`

## 端口信息
- Neo4j 浏览器: 7674
- Bolt 协议: 7689