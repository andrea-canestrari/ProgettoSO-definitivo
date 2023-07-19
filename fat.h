#ifndef fat_h
#define fat_h
#include <stddef.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

#define MAX_BLOCKS 2048
#define BLOCK_SIZE 2048
#define MAX_NAME 512
#define MAX_CHILDREN 128
#define MAX_ENTRIES 512

#define UNUSED -1
#define END_OF_CHAIN 0


#define DIRECTORY 'd'
#define FILE 'f'

#define WRITING_MODE 'w'
#define READING_MODE 'r'
#define READINGNWRITING_MODE 'x'


typedef int fat_entry;

typedef struct dir_entry{
    char name[MAX_NAME];
    char type;
    int size;
    char* creationTime;
    char* lastWriteTime;
    char* lastReadTime;
    fat_entry first_fat_block;
    int n_children;
    int children[MAX_CHILDREN];
    int parent_directory;
} dir_entry;

typedef fat_entry fat_table[MAX_BLOCKS];
typedef dir_entry dir_table[MAX_ENTRIES];
typedef char data_block[BLOCK_SIZE];

typedef struct disk_block{
    dir_table d_table;
    fat_table f_table;
    data_block data[MAX_BLOCKS];
} disk_block;

typedef struct virtual_disk{
    int size;
    disk_block* disk;
    int fd;
    int curr_dir;
} virtual_disk;

typedef struct FileHandle{
    virtual_disk* vd;
    fat_entry dir_entry;
    int pos;
    int block_index;
    char mode;
} FileHandle;

typedef dir_entry dir_array[MAX_CHILDREN];

virtual_disk* startFAT(const char* name);
FileHandle* createFile(virtual_disk* vd, const char* name, char mode);
int eraseFile(virtual_disk* vd, const char* name);
int FAT_write(FileHandle* fd, const char* buf, size_t size);
int FAT_read(FileHandle* fd, void* buf, size_t size);
int FAT_seek(FileHandle* fd, int offset, int whence);
int createDir(virtual_disk* vd, const char* name);
int changeDir(virtual_disk* vd, const char* name);
int eraseDir(virtual_disk* vd, const char* name);
dir_array* listDir(virtual_disk* vd);

#endif