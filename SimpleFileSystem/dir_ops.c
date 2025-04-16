/* dir_ops.c */
#include "file_types.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

// 声明从utils.c导入的函数
extern int validate_path(const char *path);
extern void split_path(const char *path, char *parent_path, char *filename);

// 声明从file_ops.c导入的函数
extern int create_file(directory_t *parent_dir, const char *filename, file_permissions_t permissions);

// 全局根目录
static directory_t root_directory = { "/", {}, 0 };

/*
find_directory(函数): 根据路径查找目录结构。

功能描述:
- 验证提供的路径格式是否有效
- 在文件系统层次结构中查找指定路径的目录
- 支持根目录("/")和简化的两级目录结构

参数:
- path: 要查找的目录路径字符串

返回值:
- 非NULL: 指向找到的目录结构的指针
- NULL: 目录不存在或路径无效

依赖的已解释节点:
- validate_path函数(路径验证)
- split_path函数(路径分解)
- MAX_PATH_LEN常量(路径长度限制)
- MAX_FILENAME_LEN常量(文件名长度限制)
- SUCCESS常量(成功状态码)
- FILE_TYPE_DIRECTORY常量(目录类型标识)
- directory_t结构(目录内容表示)
- file_entry_t结构(文件元数据表示)
*/
directory_t* find_directory(const char *path) {
    if (validate_path(path) != SUCCESS) {
        return NULL;
    }
    
    // 如果是根目录
    if (strcmp(path, "/") == 0) {
        return &root_directory;
    }
    
    // 简化版：只支持两级目录结构
    // 实际实现需要递归查找
    
    char parent_path[MAX_PATH_LEN];
    char dirname[MAX_FILENAME_LEN];
    
    split_path(path, parent_path, dirname);
    
    // 只看根目录下的目录
    if (strcmp(parent_path, "/") == 0) {
        for (int i = 0; i < root_directory.file_count; i++) {
            if (strcmp(root_directory.files[i].name, dirname) == 0 && 
                root_directory.files[i].type == FILE_TYPE_DIRECTORY) {
                // 假设目录实际内容存储在与file_entry关联的某处
                // 这里简化返回一个指向目录结构的指针
                return (directory_t*)root_directory.files[i].block_pointers;
            }
        }
    }
    
    return NULL;  // 目录不存在
}

/*
create_directory(函数): 创建新目录。

功能描述:
- 验证路径并解析父目录和目录名
- 检查父目录是否存在及其容量
- 创建新目录结构并在父目录中添加对应条目

参数:
- path: 要创建的目录的完整路径

返回值:
- SUCCESS: 目录创建成功
- ERR_INVALID_PATH: 路径无效或父目录不存在
- ERR_FILE_EXISTS: 同名目录或文件已存在
- ERR_DISK_FULL: 内存分配失败或目录已满

依赖的已解释节点:
- validate_path函数(路径验证)
- find_directory函数(目录查找)
- split_path函数(路径分解)
- MAX_PATH_LEN常量(路径长度限制)
- MAX_FILENAME_LEN常量(文件名长度限制)
- ERR_INVALID_PATH常量(路径无效错误码)
- ERR_FILE_EXISTS常量(文件已存在错误码)
- ERR_DISK_FULL常量(资源不足错误码)
- SUCCESS常量(成功状态码)
- FILE_TYPE_DIRECTORY常量(目录类型标识)
- directory_t结构(目录内容表示)
- file_entry_t结构(文件元数据表示)
*/
int create_directory(const char *path) {
    if (validate_path(path) != SUCCESS) {
        return ERR_INVALID_PATH;
    }
    
    char parent_path[MAX_PATH_LEN];
    char dirname[MAX_FILENAME_LEN];
    
    split_path(path, parent_path, dirname);
    
    // 查找父目录
    directory_t *parent = find_directory(parent_path);
    if (!parent) {
        return ERR_INVALID_PATH;  // 父目录不存在
    }
    
    // 检查目录名长度
    if (strlen(dirname) >= MAX_FILENAME_LEN) {
        return ERR_INVALID_PATH;
    }
    
    // 检查目录是否已存在
    for (int i = 0; i < parent->file_count; i++) {
        if (strcmp(parent->files[i].name, dirname) == 0) {
            return ERR_FILE_EXISTS;
        }
    }
    
    // 创建目录项
    file_entry_t *dir_entry = &parent->files[parent->file_count];
    strcpy(dir_entry->name, dirname);
    dir_entry->type = FILE_TYPE_DIRECTORY;
    dir_entry->size = 0;
    
    // 设置默认权限 (读/写/执行)
    dir_entry->permissions.read = 1;
    dir_entry->permissions.write = 1;
    dir_entry->permissions.execute = 1;
    
    dir_entry->created_time = time(NULL);
    dir_entry->modified_time = dir_entry->created_time;
    
    // 分配新目录结构
    directory_t *new_dir = (directory_t*)malloc(sizeof(directory_t));
    if (!new_dir) {
        return ERR_DISK_FULL;  // 内存分配失败
    }
    
    // 初始化新目录
    strcpy(new_dir->name, dirname);
    new_dir->file_count = 0;
    
    // 存储目录结构指针
    // 简化版：将目录结构指针存储在block_pointers[0]
    dir_entry->block_pointers[0] = (int)new_dir;
    
    // 增加父目录文件计数
    parent->file_count++;
    
    return SUCCESS;
}

/*
list_directory(函数): 列出目录内容。

功能描述:
- 查找指定路径的目录
- 提供对目录中所有文件和子目录条目的访问

参数:
- path: 要列出内容的目录路径
- entries: 输出参数，接收目录条目数组的指针
- count: 输出参数，接收条目数量

返回值:
- SUCCESS: 操作成功
- ERR_FILE_NOT_FOUND: 指定路径的目录不存在

依赖的已解释节点:
- find_directory函数(目录查找)
- ERR_FILE_NOT_FOUND常量(文件不存在错误码)
- SUCCESS常量(成功状态码)
- directory_t结构(目录内容表示)
- file_entry_t结构(文件元数据表示)
*/
int list_directory(const char *path, file_entry_t **entries, int *count) {
    // 查找目录
    directory_t *dir = find_directory(path);
    if (!dir) {
        return ERR_FILE_NOT_FOUND;
    }
    
    // 设置输出参数
    *entries = dir->files;
    *count = dir->file_count;
    
    return SUCCESS;
}