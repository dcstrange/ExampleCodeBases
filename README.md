
---

# Docker 容器打包与迁移工具

每个代码样例中由`*_package.tar.gz`结尾的文件，可以由本项目中的`codenexus_docker_packager.sh`脚本一件解压并执行，例如：
```bash
./codenexus_docker_packager.sh unpack <包文件名.tar.gz>
```
这是一个命令行工具，旨在简化 Docker 容器的打包、迁移和部署过程。它特别适用于那些使用了**卷挂载 (bind mounts)** 来持久化数据的容器。

此脚本可以将一个正在运行的容器（包括其所有挂载的数据和环境变量）打包成一个单一的 `.tar.gz` 压缩包。然后，您可以在任何其他装有 Docker 的机器上，使用同一个脚本轻松地解包并恢复该容器，所有数据和配置都将保持不变。

## 核心功能

-   **一键打包**：自动检测一个正在运行的容器，并将其镜像信息、环境变量和所有 `bind-mount` 的数据打包。
-   **生成启动脚本**：在包内自动生成一个可执行的 `start.sh` 脚本，该脚本包含了恢复容器所需的所有 `docker run` 命令和参数。
-   **数据可移植性**：所有挂载的数据都被复制并包含在压缩包内，使得容器及其数据可以轻松地从一台机器迁移到另一台。
-   **一键解包与部署**：自动解压、执行启动脚本、并启动容器。
-   **状态检查**：容器启动后，脚本会自动检查指定端口，以确认服务是否已成功运行。

## 系统要求

在运行此脚本之前，请确保您的系统中已安装以下依赖项：

-   **Docker**: 用于容器的运行和管理。
-   **jq**: 一个轻量级的命令行 JSON 处理器，用于解析 `docker inspect` 的输出。
-   **nc (netcat)**: 用于在容器启动后检查端口是否可访问。
-   **rsync**: 用于高效、可靠地复制目录内容。

您可以使用系统的包管理器来安装它们，例如在 Ubuntu/Debian 上：
```bash
sudo apt-get update
sudo apt-get install -y docker.io jq netcat-openbsd rsync
```

## 使用方法

将 `codenexus_docker_packager.sh` 脚本赋予执行权限：
```bash
chmod +x codenexus_docker_packager.sh
```

### 1. 打包容器 (`pack`)

此命令用于打包一个**正在运行中**的容器。

**语法:**
```bash
./codenexus_docker_packager.sh pack <正在运行的容器名>
```

**示例:**
假设您有一个名为 `my-database` 的容器正在运行，并且您想打包它。

```bash
# 执行打包命令
./codenexus_docker_packager.sh pack my-database
```

**执行过程:**
1.  脚本会检查名为 `my-database` 的容器是否存在且正在运行。
2.  它会解析该容器的配置，找到所有类型为 `bind` 的挂载点和所有环境变量。
3.  它会创建一个临时目录，并将所有挂载点主机上的数据通过 `rsync` 复制到该目录中。
4.  它会生成一个 `start.sh` 脚本，其中包含了 `docker run` 命令，并正确设置了 `-v` (卷) 和 `-e` (环境变量) 参数。
5.  最后，它会将 `start.sh` 和包含数据的 `data_mounts` 目录压缩成一个名为 `my-database_package.tar.gz` 的文件。

### 2. 解包并运行容器 (`unpack`)

此命令用于在目标机器上部署之前打包好的容器。

**语法:**
```bash
./codenexus_docker_packager.sh unpack <包文件名.tar.gz>
```

**示例:**
您已经将 `my-database_package.tar.gz` 文件和 `codenexus_docker_packager.sh` 脚本拷贝到了新的服务器上。

```bash
# 执行解包和部署命令
./codenexus_docker_packager.sh unpack my-database_package.tar.gz
```

**执行过程:**
1.  脚本会创建一个名为 `my-database_package_deploy` 的新目录。
2.  它会将 `.tar.gz` 包的内容解压到这个新目录中。
3.  它会自动执行解压出来的 `start.sh` 脚本。
4.  `start.sh` 会首先尝试停止并删除任何同名的旧容器，然后使用包内的数据和环境变量启动一个新容器。
5.  容器启动后，`codenexus_docker_packager.sh` 会开始轮询检查容器的端口，直到服务可用或超时。

## 工作流示例

**场景**: 将运行在 **服务器A** 上的 `pg17_parser` 容器迁移到 **服务器B**。

#### 步骤 1: 在服务器A上打包

1.  确保 `pg17_parser` 容器正在运行。
2.  运行打包命令：
    ```bash
    ./codenexus_docker_packager.sh pack pg17_parser
    ```
3.  命令执行成功后，您会得到一个 `pg17_parser_package.tar.gz` 文件。

#### 步骤 2: 传输文件

将 `pg17_parser_package.tar.gz` 和 `codenexus_docker_packager.sh` 这两个文件传输到 **服务器B** 的同一个目录下。

#### 步骤 3: 在服务器B上解包

1.  在服务器B上，进入存放这两个文件的目录。
2.  确保 `codenexus_docker_packager.sh` 有执行权限 (`chmod +x codenexus_docker_packager.sh`)。
3.  运行解包命令：
    ```bash
    ./codenexus_docker_packager.sh unpack pg17_parser_package.tar.gz
    ```
4.  脚本会自动完成所有部署工作。当您看到 "Deployment completed successfully!" 的消息时，`pg17_parser` 容器就已经在服务器B上带着原始数据成功运行了。所有相关文件都位于 `pg17_parser_package_deploy` 目录中。

## 自定义配置

您可以在 `codenexus_docker_packager.sh` 脚本的顶部修改一些默认配置：

-   `DEFAULT_HTTP_PORT`: 在目标机器上映射到容器 `7474` 端口的主机端口，默认为 `7674`。
-   `DEFAULT_BOLT_PORT`: 在目标机器上映射到容器 `7687` 端口的主机端口，默认为 `7689`。
-   `WAIT_TIMEOUT`: 解包后等待容器启动的超时时间（秒），默认为 `120`。

如果您需要在目标机器上使用不同的端口，请在**打包之前**修改这些值。

---