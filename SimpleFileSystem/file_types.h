/* file_types.h */
#include "constants.h"
#include <time.h>

/*
file_type_t(枚举类型): 表示文件系统中支持的不同类型的文件：
- FILE_TYPE_REGULAR: 普通文件(值为1)，用于存储用户数据
- FILE_TYPE_DIRECTORY: 目录文件(值为2)，用于组织其他文件
- FILE_TYPE_SYMLINK: 符号链接(值为3)，指向其他文件或目录的引用

依赖的已解释节点: 无（基本枚举定义）
*/
typedef enum {
    FILE_TYPE_REGULAR = 1,
    FILE_TYPE_DIRECTORY = 2,
    FILE_TYPE_SYMLINK = 3
} file_type_t;

/*
file_permissions_t(结构体): 使用位域表示Unix风格的文件权限：
- read: 1位布尔值，表示是否允许读取文件内容
- write: 1位布尔值，表示是否允许修改文件内容
- execute: 1位布尔值，表示是否允许执行文件(仅对普通文件有意义)

依赖的已解释节点: 无（基本位域结构）
*/
typedef struct {
    unsigned int read: 1;
    unsigned int write: 1;
    unsigned int execute: 1;
} file_permissions_t;

/*
file_entry_t(结构体): 表示文件系统中单个文件或目录的元数据：
- name: 文件名，最大长度由MAX_FILENAME_LEN限制
- type: 文件类型，使用file_type_t枚举
- size: 文件大小(字节数)
- permissions: 文件权限，使用file_permissions_t结构
- created_time: 文件创建时间戳
- modified_time: 文件最后修改时间戳
- block_pointers: 存储指向实际数据块的索引数组(简化实现)

依赖的已解释节点:
- MAX_FILENAME_LEN常量(文件名长度限制)
- file_type_t枚举(文件类型分类)
- file_permissions_t结构(文件权限设置)
*/
typedef struct {
    char name[MAX_FILENAME_LEN];
    file_type_t type;
    size_t size;
    file_permissions_t permissions;
    time_t created_time;
    time_t modified_time;
    int block_pointers[10];  // 简化的块指针数组
} file_entry_t;


/*
directory_t(结构体): 表示文件系统中的目录结构：
- name: 目录名称，最大长度由MAX_FILENAME_LEN限制
- files: 包含的文件和子目录数组，最大容量由MAX_FILES_PER_DIR限制
- file_count: 当前目录中实际包含的文件和子目录数量

依赖的已解释节点:
- MAX_FILENAME_LEN常量(目录名长度限制)
- MAX_FILES_PER_DIR常量(单目录最大容量)
- file_entry_t结构(文件元数据表示)
*/
typedef struct {
    char name[MAX_FILENAME_LEN];
    file_entry_t files[MAX_FILES_PER_DIR];
    int file_count;
} directory_t;