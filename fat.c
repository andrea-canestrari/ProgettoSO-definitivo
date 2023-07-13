#include "fat.h"
#define root_dir 0

char* getTime(){
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
    entry->creationTime = getModTime();
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

static int findIndex(virtual_disk* vd, const char* name, char type){
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

static void addChildren(dir_entry* parent, int child){
    for (int i=0; i<MAX_CHILDREN; i++){
        int* curr_chld = parent->children[i];
        if (curr_chld == NULL){
            *curr_chld = child;
            break;
        }
    }
    parent->n_children++;
    parent->lastWriteTime = getModTime();
    parent->lastReadTime = getModTime();
}

static void removeChild(dir_entry* parent, int child){
    for (int i=0; i<MAX_CHILDREN; i++){
        int* curr_chld = parent->children[i];
        if (*curr_chld == child){
            *curr_chld = NULL;
            break;
        }
        if (*curr_chld == NULL){
            break;
        }
    }
    parent->n_children--;
    parent->lastWriteTime = getModTime();
    parent->lastReadTime = getModTime();
}

static int createEntry(virtual_disk* vd, const char* name, int index, char type){
    int freeBlock = -1;
    if (type != DIRECTORY){
        for (int i=0; i<MAX_BLOCKS; i++){
            if (vd->disk->f_table[i] == UNUSED){
                freeBlock = i;
                break;
            }
        }
        if (freeBlock == -1){
            return -1;
        }
        vd->disk->f_table[freeBlock] = END_OF_CHAIN;
        
    }

    dir_entry* entry = (&(vd->disk->d_table[index]));
    strcpy(&entry->name, name);
    printf("entry->name == %s\n", entry->name);
    entry->type = type;
    printf("entry->type == %c\n", entry->type);
    entry->size = 0;
    printf("entry->size == %d\n", entry->size);
    entry->creationTime = getTime();
    if (entry->type == 'DIRECTORY'){
        entry->lastReadTime = 'This directory has not been accessed in reading mode yet!\n';
        entry->lastWriteTime = 'This directory has not been accessed in writing mode yet!\n';
    }
    else{
        entry->lastReadTime = 'This file has not been accessed in reading mode yet!\n';
        entry->lastWriteTime = 'This file has not been accessed in writing mode yet!\n';
    }
    
    entry->first_fat_block = freeBlock;
    printf("FAT index == %d\n", vd->disk->f_table[freeBlock]);
    entry->parent_directory = vd->curr_dir;
    printf("entry->parent_directory == %d\n", entry->parent_directory);
    addChildren((&(vd->disk->d_table[entry->parent_directory])), index);
    if (type == DIRECTORY){
        entry->n_children = 0;
        printf("entry->n_children == %d\n", entry->n_children);
        for (int i=0; i<MAX_CHILDREN; i++){
            entry->children[i] = 0;
        }
    }

    return 0;


}



FileHandle* createFile(virtual_disk* vd, const char* name, char mode){
    int file_index = findIndex(vd, name, FILE);
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

int createDir(virtual_disk* vd, const char* name){
    int dir_index = findIndex(vd, name, DIRECTORY);
    int free_index = findFreeIndex(vd, name);
    printf("dir_index == %d,  free_index ==%d\n", dir_index, free_index);
    if (dir_index == -1 && free_index == -1){
        printf("Not enough space on the disk! (used ==-1, not_used ==-1)\n");
        return NULL;
    }
    if (dir_index != -1){
        return 0;
    }
    return createEntry(vd, name, free_index, DIRECTORY);
}

void killFile(FileHandle* file){
    free(file);
}


int eraseFile(virtual_disk* vd, const char* name){
    int index = findIndex(vd, name, FILE);
    printf("%d\n", index);
    if (index == -1){
        printf("No such file or directory!\n");
        return -1;
    }
    dir_entry* entry = &(vd->disk->d_table[index]);
    printf("%s\n", entry->name);
    int curr_e = entry->first_fat_block;
    while (curr_e != END_OF_CHAIN){
        if (curr_e != UNUSED){
            int new_e = vd->disk->f_table[curr_e];
            vd->disk->f_table[curr_e] = UNUSED;
            curr_e = new_e;
        }
        else{
            return -1;
        }
    }
    if (entry->parent_directory != root_dir){
        removeChild(&(vd->disk->d_table[entry->parent_directory]), index);
    }
    memset(entry, 0, sizeof(dir_entry));
    return 0;
}

int eraseDir(virtual_disk* vd, const char* name){
    int index= findEntry(vd, name, DIRECTORY);
    if (index == -1){
        printf("Directory not found!\n");
        return -1;
    }
    dir_entry* entry = &(vd->disk->d_table[index]);
    if (entry->n_children > 0){
        printf("Entry is a file!\n");
        return -1;
    }
    if (entry->parent_directory != root_dir){
        removeChild(&(vd->disk->d_table[entry->parent_directory]), index);
    }
    memset(entry, 0, sizeof(dir_entry));
    return 0;
}

int changeDir(virtual_disk* vd, const char* name){
    printf("prev_dir == %d\n", vd->curr_dir);
    if (name == '..'){
        if (vd->curr_dir == root_dir){
            printf("Already in root dir!\n");
            return -1;
        }
        vd->curr_dir = (vd->disk->d_table[vd->curr_dir]).parent_directory;
        return 0;
    }
    else if (name == '/'){
        vd->curr_dir = root_dir;
        return 0;
    }

    else{
        int index = findEntry(vd, name, DIRECTORY);
        if (index == -1){
            printf("Directory not found!\n");
            return -1;
        }
        vd->curr_dir = index;
    }

    printf("curr_dir == %d\n", vd->curr_dir);
    return 0;

}

dir_array* listDir(virtual_disk* vd){
    dir_entry* curr_dir = &(vd->disk->d_table[vd->curr_dir]);
    dir_array* res = (dir_array*)malloc(sizeof(dir_entry)*curr_dir->n_children);
    if (curr_dir->name!='\0' && curr_dir->type=='d')
    printf("current dir == %s\n", curr_dir->name);
    printf("size == %d\n", curr_dir->size);
    printf("type == %c\n", curr_dir->type);
    printf("creation time == %s\n", curr_dir->creationTime);
    printf("last write time == %s\n", curr_dir->lastReadTime);
    printf("last read time == %s\n", curr_dir->lastWriteTime);
    printf("first block in FAT table == %d\n",curr_dir->first_fat_block);
    printf("no. of children == %d\n", curr_dir->n_children);
    printf("parent directory == %d\n", curr_dir->parent_directory);
    printf("\n");

    for (int i=0; i< curr_dir->n_children; i++){
        dir_entry* curr_chld = &(vd->disk->d_table[curr_dir->children[i]]);
        if (curr_chld->name!='\0' && (curr_chld->type=='d' || curr_chld->type=='f')){
            printf("\n");
            printf("current dir/file == %s\n",curr_chld->name);
            printf("size of file == %d\n", curr_chld->size);
            printf("type of file == %c\n", curr_chld->type);
            printf("creation time == %s\n", curr_chld->creationTime);
            printf("last write time == %s\n", curr_chld->lastReadTime);
            printf("last read time == %s\n", curr_chld->lastWriteTime);
            printf("first block in FAT table == %d\n",curr_chld->first_fat_block);
            printf("no. of children == %d\n", curr_chld->n_children);
            printf("parent directory == %d\n", curr_chld->parent_directory); 
            memmove(res[i], curr_chld, sizeof(dir_entry));
            printf("memmove no.%d complete!\n", i);
            printf("\n");
           }
    }
    return res;
  }
