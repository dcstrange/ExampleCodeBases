/* utils.c */
#include "file_types.h"
#include <string.h>
#include <stdio.h>

/*validate_path(函数): 验证文件路径的有效性。
- 检查路径非空且长度未超过MAX_PATH_LEN
- 验证路径不包含Windows文件系统禁止的特殊字符(<, >, |, *, ?)
- 成功返回SUCCESS，失败返回ERR_INVALID_PATH

依赖的已解释节点:
- MAX_PATH_LEN常量(路径长度上限)
- SUCCESS常量(成功状态码)
- ERR_INVALID_PATH常量(路径无效错误码)
*/
int validate_path(const char *path) {
    if (!path || strlen(path) >= MAX_PATH_LEN) {
        return ERR_INVALID_PATH;
    }
    
    // 检查路径中是否包含非法字符
    for (int i = 0; path[i] != '\0'; i++) {
        if (path[i] == '<' || path[i] == '>' || path[i] == '|' || path[i] == '*' || path[i] == '?') {
            return ERR_INVALID_PATH;
        }
    }
    
    return SUCCESS;
}

/*
split_path(函数): 将完整文件路径分解为父目录路径和文件名。
- 查找最后一个斜杠位置确定分割点
- 提取父目录路径(斜杠前的部分)
- 提取文件名(最后一个斜杠后的部分)
- 如无斜杠，假定为当前目录('.')中的文件

依赖的已解释节点: 无(基本字符串操作)
*/
void split_path(const char *path, char *parent_path, char *filename) {
    const char *last_slash = strrchr(path, '/');
    
    if (last_slash) {
        int parent_len = last_slash - path;
        strncpy(parent_path, path, parent_len);
        parent_path[parent_len] = '\0';
        strcpy(filename, last_slash + 1);
    } else {
        // 没有斜杠表示当前目录中的文件
        parent_path[0] = '.';
        parent_path[1] = '\0';
        strcpy(filename, path);
    }
}

/*
allocate_blocks(函数): 分配指定数量的存储块用于文件数据存储。
- 实现简化版本，使用静态变量跟踪下一个可用块
- 检查剩余空间是否足够满足请求
- 成功返回分配的第一个块索引，失败返回ERR_DISK_FULL

依赖的已解释节点:
- MAX_FILE_SIZE常量(文件大小上限)
- BLOCK_SIZE常量(存储块大小)
- ERR_DISK_FULL常量(磁盘空间不足错误码)
*/
int allocate_blocks(int num_blocks) {
    // 简化版：假设返回第一个可用块的索引
    static int next_free_block = 0;
    int start_block = next_free_block;
    
    // 检查是否有足够的块可用
    if (next_free_block + num_blocks > MAX_FILE_SIZE / BLOCK_SIZE) {
        return ERR_DISK_FULL;
    }
    
    next_free_block += num_blocks;
    return start_block;
}