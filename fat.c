#include "fat.h"
#define root_dir 0

char* getModTime(){
    time_t mytime = time(NULL);
    char * time_str = ctime(&mytime);
    time_str[strlen(time_str)-1] = '\0';
    return time_str;
}

virtual_disk* startFAT(const char* name){
    virtual_disk* vd=(virtual_disk*)malloc(sizeof(virtual_disk));
    if (vd==NULL){
        printf("Cannot allocate memory for virtual disk structure!\n");
        return NULL;
    }
    printf("malloc ok!\n");
    vd->fd = open(name, O_CREAT|O_RDWR, 0666);
    if (vd->fd == -1){
        printf("Cannot open the file descriptor linked with the virtual disk!\n");
        return NULL;
    }
    printf("open ok!\n");
    if (ftruncate(vd->fd, sizeof(disk_block)) != 0){
        printf("Cannot size the file descriptor!\n");
        return NULL;
    }
    vd->disk = (disk_block*) mmap(NULL,sizeof(disk_block), PROT_READ|PROT_WRITE, MAP_SHARED, vd->fd, 0);
    if (vd->disk == MAP_FAILED){
        printf("Cannot map memory needed for disk!\n");
        return NULL;
    }
    printf("mmap ok!\n");
    memset(&(vd->disk->f_table), UNUSED, sizeof(fat_table));
    printf("memset ok!\n");
    dir_entry* entry = (&(vd->disk->d_table[root_dir]));
    entry->name[0] = '/';
    entry->name[1] = '\0';
    entry->type = DIRECTORY;
    entry->mod_time = getModTime();
    entry->n_children = 0;
    entry->parent_directory=-1;
    printf("setup ok!\n");
    vd->size = sizeof(fat_table) + sizeof(data_block) * MAX_BLOCKS;
    vd->curr_dir = 0;
    printf("vd->size == %d\n", vd->size);
    printf("vd->curr_dir == %d\n", vd->curr_dir);
    printf("vd->fd == %d\n", vd->fd);
    printf("vd->disk == %p\n", vd->disk);
    return vd;
}

int killFAT(virtual_disk* vd){
   int res= munmap(vd->disk, vd->size);
   if (res == -1){
     printf("Cannot deallocate disk!\n");
     return -1;
   }
   res = close(vd->fd);
   if (res == -1){
     printf("Cannot close file descriptor!\n");
     return -1;
    }
   free(vd);
   return res;
}

static int findFile(virtual_disk* vd, const char* name, char type){
    int index = -1;
    for (int i=1; i<MAX_ENTRIES; i++){
        dir_entry* entry = (&(vd->disk->d_table[i]));
        if (entry->parent_directory == vd->curr_dir && strcmp(name, entry->name) == 0){
            if (entry->type != type){
                index=-1;
                break;
            }
            return index;
        }
    }
    if ((vd->disk->d_table[vd->curr_dir].n_children) >= MAX_CHILDREN){
        index = -1;
    }

    return index;
}

static int findFreeIndex(virtual_disk* vd, const char* name){
    int index = -1;
    for (int i=1; i<MAX_ENTRIES; i++){
        dir_entry* entry = (&(vd->disk->d_table[i]));
        if (entry->name == NULL || entry->name[0] == '\0'){
            if (index == -1){
                index = i;
            }
        }

    return index;    
    }
}



FileHandle* createFile(virtual_disk* vd, const char* name, const mode){
    int file_index = findFile(vd, name, FILE);
    int free_index = findFreeIndex(vd, name);
    printf("file_index == %d,  free_index ==%d\n", file_index, free_index);
    if (file_index == -1 && free_index == -1){
        printf("Not enough space on the disk! (used ==-1, not_used ==-1)\n");
        return NULL;
    }
    FileHandle* file=(FileHandle*)malloc(sizeof(FileHandle));
    if (file_index!=-1){
        file->block_index = 0;
        file->pos = 0;
        file->dir_entry = file_index;
        file->mode = mode;
    }
    else{
        if (createEntry(vd, name, free_index, FILE) == -1){
            free(file);
            printf("Not enough space on the disk!\n");
            return NULL;
        }
        file->block_index = 0;
        file->pos = 0;
        file->dir_entry = free_index;
        file->mode = mode;
    }
    file->vd = vd;
    printf("file->block_index == %d\n", file->block_index);
    printf("file->pos == %d\n", file->pos);
    printf("file->dir_entry == %d\n", file->dir_entry);
    printf("file->disk == %p\n", file->vd);
    printf("file->mode == %c\n", file->mode);
    return file;

}
