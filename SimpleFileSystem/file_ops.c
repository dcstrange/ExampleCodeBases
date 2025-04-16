/* file_ops.c */
#include "file_types.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

// 声明从utils.c导入的函数
extern int validate_path(const char *path);
extern int allocate_blocks(int num_blocks);

/*
create_file(函数): 在指定目录中创建新的空文件。

功能描述:
- 在父目录中创建具有指定文件名和权限的新文件
- 初始化文件元数据，包括类型、大小和时间戳
- 文件创建时不分配数据块，初始大小为0

参数:
- parent_dir: 要在其中创建文件的目录
- filename: 新文件的名称
- permissions: 要应用的读/写/执行权限

返回值:
- SUCCESS: 文件创建成功
- ERR_DISK_FULL: 目录已达到最大文件数限制
- ERR_INVALID_PATH: 文件名长度超过限制
- ERR_FILE_EXISTS: 同名文件已存在

依赖的已解释节点:
- MAX_FILES_PER_DIR常量(目录容量限制)
- MAX_FILENAME_LEN常量(文件名长度限制)
- ERR_DISK_FULL常量(空间不足错误码)
- ERR_INVALID_PATH常量(路径无效错误码)
- ERR_FILE_EXISTS常量(文件已存在错误码)
- SUCCESS常量(成功状态码)
- file_entry_t结构(文件元数据表示)
- FILE_TYPE_REGULAR常量(常规文件类型标识)
- directory_t结构(目录内容表示)
*/
int create_file(directory_t *parent_dir, const char *filename, file_permissions_t permissions) {
    // 检查目录是否已满
    if (parent_dir->file_count >= MAX_FILES_PER_DIR) {
        return ERR_DISK_FULL;
    }
    
    // 检查文件名长度
    if (strlen(filename) >= MAX_FILENAME_LEN) {
        return ERR_INVALID_PATH;
    }
    
    // 检查文件是否已存在
    for (int i = 0; i < parent_dir->file_count; i++) {
        if (strcmp(parent_dir->files[i].name, filename) == 0) {
            return ERR_FILE_EXISTS;
        }
    }
    
    // 创建新文件项
    file_entry_t *new_file = &parent_dir->files[parent_dir->file_count];
    strcpy(new_file->name, filename);
    new_file->type = FILE_TYPE_REGULAR;
    new_file->size = 0;
    new_file->permissions = permissions;
    new_file->created_time = time(NULL);
    new_file->modified_time = new_file->created_time;
    
    // 增加目录文件计数
    parent_dir->file_count++;
    
    return SUCCESS;
}

/*
read_file(函数): 从文件中读取数据。

功能描述:
- 在目录中定位指定文件
- 验证请求的读取权限和范围
- 从文件的指定偏移位置读取数据

参数:
- parent_dir: 包含目标文件的目录
- filename: 要读取的文件名
- buffer: 存放读取数据的内存缓冲区
- size: 要读取的最大字节数
- offset: 文件内开始读取的位置(字节偏移)

返回值:
- 正数: 实际读取的字节数
- ERR_FILE_NOT_FOUND: 指定文件不存在
- ERR_PERMISSION_DENIED: 缺少读取权限

依赖的已解释节点:
- ERR_FILE_NOT_FOUND常量(文件不存在错误码)
- ERR_PERMISSION_DENIED常量(权限不足错误码)
- file_entry_t结构(文件元数据表示)
- FILE_TYPE_REGULAR常量(常规文件类型标识)
- directory_t结构(目录内容表示)
*/
int read_file(directory_t *parent_dir, const char *filename, void *buffer, size_t size, size_t offset) {
    // 查找文件
    file_entry_t *file = NULL;
    for (int i = 0; i < parent_dir->file_count; i++) {
        if (strcmp(parent_dir->files[i].name, filename) == 0 && 
            parent_dir->files[i].type == FILE_TYPE_REGULAR) {
            file = &parent_dir->files[i];
            break;
        }
    }
    
    if (!file) {
        return ERR_FILE_NOT_FOUND;
    }
    
    // 检查读取权限
    if (!file->permissions.read) {
        return ERR_PERMISSION_DENIED;
    }
    
    // 检查读取范围是否有效
    if (offset >= file->size) {
        return 0;  // 没有可读取的内容
    }
    
    // 计算实际可读取的字节数
    size_t bytes_to_read = (offset + size <= file->size) ? size : (file->size - offset);
    
    // 简化版：实际应该从block_pointers指向的块中读取数据
    // 这里只是示例，所以返回可读取的字节数
    
    return bytes_to_read;
}

/*
write_file(函数): 向文件写入数据。

功能描述:
- 在目录中定位指定文件
- 验证写入权限
- 必要时扩展文件大小并分配新存储块
- 在指定偏移处写入数据并更新文件元数据

参数:
- parent_dir: 包含目标文件的目录
- filename: 要写入的文件名
- buffer: 包含要写入数据的内存缓冲区
- size: 要写入的字节数
- offset: 文件内开始写入的位置(字节偏移)

返回值:
- 正数: 实际写入的字节数
- ERR_FILE_NOT_FOUND: 指定文件不存在
- ERR_PERMISSION_DENIED: 缺少写入权限
- ERR_DISK_FULL: 存储空间不足或文件大小超限

依赖的已解释节点:
- ERR_FILE_NOT_FOUND常量(文件不存在错误码)
- ERR_PERMISSION_DENIED常量(权限不足错误码)
- ERR_DISK_FULL常量(空间不足错误码)
- MAX_FILE_SIZE常量(文件大小上限)
- BLOCK_SIZE常量(存储块大小)
- file_entry_t结构(文件元数据表示)
- FILE_TYPE_REGULAR常量(常规文件类型标识)
- directory_t结构(目录内容表示)
- allocate_blocks函数(存储空间分配)
*/
int write_file(directory_t *parent_dir, const char *filename, const void *buffer, size_t size, size_t offset) {
    // 查找文件
    file_entry_t *file = NULL;
    for (int i = 0; i < parent_dir->file_count; i++) {
        if (strcmp(parent_dir->files[i].name, filename) == 0 && 
            parent_dir->files[i].type == FILE_TYPE_REGULAR) {
            file = &parent_dir->files[i];
            break;
        }
    }
    
    if (!file) {
        return ERR_FILE_NOT_FOUND;
    }
    
    // 检查写入权限
    if (!file->permissions.write) {
        return ERR_PERMISSION_DENIED;
    }
    
    // 检查是否需要扩展文件大小
    size_t new_size = (offset + size > file->size) ? (offset + size) : file->size;
    
    // 检查文件大小上限
    if (new_size > MAX_FILE_SIZE) {
        return ERR_DISK_FULL;
    }
    
    // 如果需要分配新块
    if (new_size > file->size) {
        int blocks_needed = (new_size - file->size + BLOCK_SIZE - 1) / BLOCK_SIZE;
        int start_block = allocate_blocks(blocks_needed);
        if (start_block < 0) {
            return start_block;  // 返回错误码
        }
        
        // 简化版：应该更新file->block_pointers，分配新块
    }
    
    // 更新文件大小和修改时间
    file->size = new_size;
    file->modified_time = time(NULL);
    
    return size;  // 返回写入的字节数
}