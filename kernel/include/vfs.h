#pragma once

#include <stddef.h>
#include <stdint.h>

#define FS_FILE         0x01
#define FS_DIRECTORY    0x02
#define FS_CHARDEVICE   0x03
#define FS_BLOCKDEVICE  0x04
#define FS_PIPE         0x05
#define FS_SYMLINK      0x06
#define FS_MOUNTPOINT   0x08

typedef uint64_t (*fs_func_read_t)(struct fs_node*, uint32_t, uint32_t, uint8_t*);
typedef uint64_t (*fs_func_write_t)(struct fs_node*, uint32_t, uint32_t, uint8_t*);
typedef void (*fs_func_open_t)(struct fs_node*);
typedef void (*fs_func_close_t)(struct fs_node*);
typedef struct dirent* (*fs_func_readdir_t)(struct fs_node*, uint32_t);
typedef struct fs_node* (*fs_func_finddir_t)(struct fs_node*, uint8_t* name);

struct dirent
{
    uint8_t* name[128];
    uint32_t inode_num;
};

typedef struct fs_node
{   
    uint8_t* name[128];
    uint64_t flags;
    uint64_t mask;
    uint64_t type;
    uint64_t length;
    uint64_t reserved;
    fs_func_read_t read;
    fs_func_write_t write;
    fs_func_open_t open;
    fs_func_close_t close;
    fs_func_readdir readdir;
    fs_func_finddir finddir;
    fs_node_t* ptr;
}fs_node_t;

