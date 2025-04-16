/*
MAX_FILENAME_LEN(常量): 定义文件名最大长度为256字符，限制单个文件或目录名称的字符数量。
*/
#define MAX_FILENAME_LEN 256

/*
MAX_PATH_LEN(常量): 定义完整文件路径最大长度为1024字符，限制文件系统中任何路径的总字符数。
*/
#define MAX_PATH_LEN 1024

/*MAX_FILES_PER_DIR(常量): 定义单个目录可包含的最大文件或子目录数为100，限制目录项数量。
*/
#define MAX_FILES_PER_DIR 100

/*MAX_FILE_SIZE(常量): 定义单个文件最大尺寸为1MB(1048576字节)，设置文件系统支持的文件大小上限。
*/
#define MAX_FILE_SIZE 1048576  // 1MB

/*BLOCK_SIZE(常量): 定义文件系统基本存储块大小为4KB(4096字节)，影响磁盘空间分配的粒度。
*/
#define BLOCK_SIZE 4096

/*
fs_error_t(枚举类型): 定义文件系统操作的可能错误码：
- SUCCESS: 操作成功完成(0)
- ERR_FILE_NOT_FOUND: 请求的文件或目录不存在(-1)
- ERR_FILE_EXISTS: 尝试创建的文件或目录已存在(-2)
- ERR_DISK_FULL: 存储空间不足，无法完成操作(-3)
- ERR_INVALID_PATH: 提供的路径格式无效或超出长度限制(-4)
- ERR_PERMISSION_DENIED: 用户权限不足，无法执行请求操作(-5)
*/
typedef enum {
    SUCCESS = 0,
    ERR_FILE_NOT_FOUND = -1,
    ERR_FILE_EXISTS = -2,
    ERR_DISK_FULL = -3,
    ERR_INVALID_PATH = -4,
    ERR_PERMISSION_DENIED = -5
} fs_error_t;