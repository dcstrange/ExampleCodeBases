/* fs_api.c */
#include "file_types.h"
#include <string.h>
#include <stdio.h>

// 声明从utils.c导入的函数
extern int validate_path(const char *path);
extern void split_path(const char *path, char *parent_path, char *filename);

// 声明从file_ops.c导入的函数
extern int create_file(directory_t *parent_dir, const char *filename, file_permissions_t permissions);
extern int read_file(directory_t *parent_dir, const char *filename, void *buffer, size_t size, size_t offset);
extern int write_file(directory_t *parent_dir, const char *filename, const void *buffer, size_t size, size_t offset);

// 声明从dir_ops.c导入的函数
extern directory_t* find_directory(const char *path);
extern int create_directory(const char *path);
extern int list_directory(const char *path, file_entry_t **entries, int *count);

// 文件系统API函数

/*
fs_init(函数): 初始化文件系统。

功能描述:
- 准备文件系统以供使用
- 显示关键配置参数如块大小和最大文件大小
- 设置初始状态和数据结构

返回值:
- SUCCESS: 初始化成功

依赖的已解释节点:
- BLOCK_SIZE常量(存储块大小)
- MAX_FILE_SIZE常量(文件大小上限)
- SUCCESS常量(成功状态码)
*/
int fs_init() {
    // 初始化文件系统
    printf("File system initialized with block size: %d bytes\n", BLOCK_SIZE);
    printf("Maximum file size: %d bytes\n", MAX_FILE_SIZE);
    
    return SUCCESS;
}

/*
fs_create_file(函数): 在指定路径创建新文件。

功能描述:
- 提供面向路径的文件创建接口
- 解析路径以确定父目录和文件名
- 设置指定的读写执行权限
- 在适当的目录中创建文件

参数:
- path: 要创建的文件完整路径
- read_perm: 读取权限标志(1=允许,0=禁止)
- write_perm: 写入权限标志(1=允许,0=禁止)
- exec_perm: 执行权限标志(1=允许,0=禁止)

返回值:
- SUCCESS: 文件创建成功
- ERR_INVALID_PATH: 路径无效
- ERR_FILE_EXISTS: 文件已存在
- 其他错误码: 继承自底层函数

依赖的已解释节点:
- validate_path函数(路径验证)
- split_path函数(路径分解)
- find_directory函数(目录查找)
- create_file函数(文件创建)
- MAX_PATH_LEN常量(路径长度限制)
- MAX_FILENAME_LEN常量(文件名长度限制)
- ERR_INVALID_PATH常量(路径无效错误码)
- file_permissions_t结构(权限表示)
*/
int fs_create_file(const char *path, int read_perm, int write_perm, int exec_perm) {
    if (validate_path(path) != SUCCESS) {
        return ERR_INVALID_PATH;
    }
    
    char parent_path[MAX_PATH_LEN];
    char filename[MAX_FILENAME_LEN];
    
    split_path(path, parent_path, filename);
    
    // 查找父目录
    directory_t *parent = find_directory(parent_path);
    if (!parent) {
        return ERR_INVALID_PATH;  // 父目录不存在
    }
    
    // 设置权限
    file_permissions_t perms;
    perms.read = read_perm ? 1 : 0;
    perms.write = write_perm ? 1 : 0;
    perms.execute = exec_perm ? 1 : 0;
    
    // 创建文件
    return create_file(parent, filename, perms);
}

/*
fs_read(函数): 从文件读取数据。

功能描述:
- 提供面向路径的文件读取接口
- 解析路径以定位目标文件
- 从指定偏移处读取请求的数据量

参数:
- path: 目标文件的完整路径
- buffer: 存储读取数据的内存缓冲区
- size: 要读取的最大字节数
- offset: 文件中的起始位置

返回值:
- 正数: 实际读取的字节数
- ERR_INVALID_PATH: 路径格式无效
- ERR_FILE_NOT_FOUND: 文件或父目录不存在
- 其他错误码: 继承自底层函数

依赖的已解释节点:
- validate_path函数(路径验证)
- split_path函数(路径分解)
- find_directory函数(目录查找)
- read_file函数(文件读取)
- MAX_PATH_LEN常量(路径长度限制)
- MAX_FILENAME_LEN常量(文件名长度限制)
- ERR_INVALID_PATH常量(路径无效错误码)
- ERR_FILE_NOT_FOUND常量(文件不存在错误码)
*/
int fs_read(const char *path, void *buffer, size_t size, size_t offset) {
    if (validate_path(path) != SUCCESS) {
        return ERR_INVALID_PATH;
    }
    
    char parent_path[MAX_PATH_LEN];
    char filename[MAX_FILENAME_LEN];
    
    split_path(path, parent_path, filename);
    
    // 查找父目录
    directory_t *parent = find_directory(parent_path);
    if (!parent) {
        return ERR_FILE_NOT_FOUND;  // 父目录不存在
    }
    
    // 读取文件
    return read_file(parent, filename, buffer, size, offset);
}

/*
fs_write(函数): 向文件写入数据。

功能描述:
- 提供面向路径的文件写入接口
- 解析路径以定位目标文件
- 在指定偏移处写入数据

参数:
- path: 目标文件的完整路径
- buffer: 包含要写入数据的内存缓冲区
- size: 要写入的字节数
- offset: 文件中的起始位置

返回值:
- 正数: 实际写入的字节数
- ERR_INVALID_PATH: 路径格式无效
- ERR_FILE_NOT_FOUND: 文件或父目录不存在
- 其他错误码: 继承自底层函数

依赖的已解释节点:
- validate_path函数(路径验证)
- split_path函数(路径分解)
- find_directory函数(目录查找)
- write_file函数(文件写入)
- MAX_PATH_LEN常量(路径长度限制)
- MAX_FILENAME_LEN常量(文件名长度限制)
- ERR_INVALID_PATH常量(路径无效错误码)
- ERR_FILE_NOT_FOUND常量(文件不存在错误码)
*/
int fs_write(const char *path, const void *buffer, size_t size, size_t offset) {
    if (validate_path(path) != SUCCESS) {
        return ERR_INVALID_PATH;
    }
    
    char parent_path[MAX_PATH_LEN];
    char filename[MAX_FILENAME_LEN];
    
    split_path(path, parent_path, filename);
    
    // 查找父目录
    directory_t *parent = find_directory(parent_path);
    if (!parent) {
        return ERR_FILE_NOT_FOUND;  // 父目录不存在
    }
    
    // 写入文件
    return write_file(parent, filename, buffer, size, offset);
}

/*
fs_mkdir(函数): 创建新目录。

功能描述:
- 提供直接的目录创建接口
- 将请求委托给底层目录创建功能

参数:
- path: 要创建的目录的完整路径

返回值:
- SUCCESS: 目录创建成功
- 错误码: 继承自底层create_directory函数

依赖的已解释节点:
- create_directory函数(目录创建)

*/
int fs_mkdir(const char *path) {
    return create_directory(path);
}

/*
fs_list(函数): 列出目录内容。

功能描述:
- 提供直接的目录内容列举接口
- 将请求委托给底层目录列举功能

参数:
- path: 要列出内容的目录路径
- entries: 输出参数，将接收目录条目数组指针
- count: 输出参数，将接收条目数量

返回值:
- SUCCESS: 操作成功
- 错误码: 继承自底层list_directory函数

依赖的已解释节点:
- list_directory函数(目录内容列举)
*/
int fs_list(const char *path, file_entry_t **entries, int *count) {
    return list_directory(path, entries, count);
}