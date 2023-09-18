#include "fat.h"
#define root_dir 0
#include <time.h>

char* getTime(){
    time_t mytime = time(NULL);
    char * time_str = ctime(&mytime);
    time_str[strlen(time_str)-1] = '\0';
    return time_str;
}


static void addChild(dir_entry* parent, int child){
    
    for (int i=0; i<MAX_CHILDREN; i++){
        int* child_i = &(parent->children[i]);
        if (*child_i == 0 ){
            *child_i = child;
            break;
        }
    }
    parent->n_children++;
    parent->lastWriteTime = getTime();
    parent->lastReadTime = getTime();
}



static void removeChild(dir_entry* parent, int child){
    for (int i=0; i<MAX_CHILDREN; i++){
        int* child_i = &(parent->children[i]);
        if (*child_i == child){
            *child_i = 0;
            break;
        }
        if (*child_i == 0){
            break;
        }
    }
    parent->n_children--;
    parent->lastWriteTime = getTime();
    parent->lastReadTime = getTime();
}


static int createEntry(virtual_disk* vd,const char* name, int id, char type){
    int freeBlock;
        freeBlock= findFreeFATBlock(vd);
        if (freeBlock == -1){
            return -1;
        }
        vd->disk->f_table[freeBlock] = END_OF_CHAIN;
        dir_entry* entry= (&(vd->disk->d_table[id]));
        strcpy(&entry->name[0], name);
        printf("entry->name == %s\n", entry->name);
        entry->first_fat_block = freeBlock;
        printf("entry->first_fat_block == %d\n", entry->first_fat_block);
        entry->size = 0;
        printf("entry->size == %d\n", entry->size);
        entry->type = type;
        printf("entry->type == %c\n", entry->type);
        entry->creationTime = getTime();
        printf("entry->creationTime == %s\n", entry->creationTime);
        if (type == DIRECTORY){
           entry->lastWriteTime = getTime();
           printf("\n");
           entry->lastReadTime = getTime();
	}
        if (type == FILE){
           entry->lastWriteTime = "This file has not been accessed in writing mode yet!\n";
           entry->lastReadTime = "This file has not been accessed in reading mode yet!\n";
	}
        printf("entry->lastWriteTime == %s\n", entry->lastWriteTime);
        printf("entry->lastReadTime == %s\n", entry->lastReadTime); 	
        entry->parent_directory = vd->curr_dir;
        printf("entry->parent_directory == %d\n", entry->parent_directory);
        addChild((&(vd->disk->d_table[entry->parent_directory])), id);
        if (type == DIRECTORY){
            entry->n_children = 0;
            printf("entry->n_children == %d\n", entry->n_children);
            memset(entry->children, 0, sizeof(entry->children));
        }
        return 0;
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
            index = i;
            entry->lastWriteTime = getTime();
            entry->lastReadTime = getTime();
	    return i;

            
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
                break;
            }
        }
        else{
          continue;
         }

    return index;    
    }
}

int findFreeFATBlock(virtual_disk* vd){
    for (int i=0; i<MAX_BLOCKS; i++){
        if (vd->disk->f_table[i] == UNUSED){
            return i;
        }
    }
    return -1;
}

void changeMod(FileHandle f, char newMode){
    if (newMode == WRITING_MODE || newMode == READING_MODE || newMode == READINGNWRITING_MODE){
        f.mode = newMode;
    }
    else{
        printf("Cannot recognize mode!\n");
        return;
    }
}

 data_block* getFreeDataBlock(virtual_disk* vd, int fat_index){
    int next_index = vd->disk->f_table[fat_index];
    if (next_index != END_OF_CHAIN){
        fat_index = next_index;
        return &(vd->disk->f_table[next_index]);
    }
    int new_index = findFreeFATBlock(vd);
    if (new_index == -1){
        return NULL;
    }
    vd->disk->f_table[fat_index] = new_index;
    fat_index = new_index;
    vd->disk->f_table[new_index] = 0;
    data_block* data = (&(vd->disk->data[new_index]));
    memset(data, 0, sizeof(data));
    return data;
}
  data_block* getDataBlock(FileHandle* fd, int fat_index){
    virtual_disk* vd = fd->vd;
    dir_entry* entry = &vd->disk->d_table[fd->dir_entry];
    int curr_fat_index = entry->first_fat_block;
    for (int i=0; i<fd->block_index; i++){
        if (curr_fat_index != UNUSED){
            if (curr_fat_index == END_OF_CHAIN){
                return NULL;
            }
            curr_fat_index = vd->disk->f_table[curr_fat_index];
        }
        
    }
    fat_index = curr_fat_index;
    return &(vd->disk->data[curr_fat_index]);
  }
  data_block* searchFreeDataBlock(FileHandle* fd, int fat_index){
    fd->block_index--;
    data_block* d_block = getDataBlock(fd, fat_index);
    if (d_block != NULL){
        fd->block_index ++;
        if ((getFreeDataBlock(fd->vd, fat_index)) == NULL){
            printf("Not enough space on the virtual disk!\n");
            return NULL;
        }
        fd->pos = 0;
        return d_block;
    }
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
    entry->creationTime = getTime();
    entry->lastWriteTime = getTime();
    entry->lastReadTime = getTime();
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

FileHandle* createFile(virtual_disk* vd, const char* name, char mode){
    int file_index = findIndex(vd, name, FILE);
    int free_index = findFreeIndex(vd, name);
    printf("file_index == %d,  free_index ==%d\n", file_index, free_index);
    if (file_index == -1 && free_index == -1){
        printf("Not enough space on the disk! (used ==-1, not_used ==-1)\n");
        return NULL;
    }
    FileHandle* file=(FileHandle*)malloc(sizeof(FileHandle));
    printf("malloc ok!\n");
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
        printf("createEntry ok!\n");
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

void killFile(FileHandle* file){
    free(file);
}

int createDir(virtual_disk* vd, const char* name){
    int free_index = findFreeIndex(vd, name);
    int file_index = findIndex(vd, name, DIRECTORY);
    printf("file_index == %d, free_index == %d\n", file_index, free_index);
    if (file_index == -1 && free_index == -1){
        printf("Not enough space on the disk!\n");
        return -1;
    }
    if (file_index != -1){
        return 0;
    }
    return createEntry(vd, name, free_index, DIRECTORY);
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
    while (curr_e != 0){
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
    int index= findIndex(vd, name, DIRECTORY);
    if (index == -1){
        printf("Directory not found!\n");
        return -1;
    }
    dir_entry* entry = &(vd->disk->d_table[index]);
    int curr_e = entry->first_fat_block;
    while (curr_e != 0){
        if (curr_e != UNUSED){
            int new_e = vd->disk->f_table[curr_e];
            vd->disk->f_table[curr_e] = UNUSED;
            curr_e = new_e;
        }
        else{
            return -1;
        }
    }
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
    printf("curr_dir prev == %d\n", vd->curr_dir);
    if (name[0] == '.' && name[1] == '.' && name[2]=='\0'){
        if (vd->curr_dir == root_dir){
            printf("Already in root dir!\n");
            return -1;
        }
        vd->curr_dir = (vd->disk->d_table[vd->curr_dir]).parent_directory;
        return 0;
    }
    if ((name[0] == '/') && name[1] == '\0'){
        vd->curr_dir = root_dir;
        return 0;
    }
    int index = findIndex(vd, name, DIRECTORY);
    
    if (index == -1){
        printf("Entry not found!\n");
        return -1;
    }
    vd->curr_dir = index;
    printf("curr_dir now == %d\n", vd->curr_dir);
    return 0;
}

dir_array* listDir(virtual_disk* vd){
    dir_entry* curr_dir = &(vd->disk->d_table[vd->curr_dir]);
    dir_array* res = (dir_array*)malloc(MAX_ENTRIES*sizeof(dir_entry));
    if (curr_dir->name!='\0' && curr_dir->type=='d'){
    printf("current dir == %s\n", curr_dir->name);
    printf("first block in FAT table == %d\n",curr_dir->first_fat_block);
    printf("creation time == %s\n", curr_dir->creationTime);
    printf("last writing time == %s\n", curr_dir->lastWriteTime);
    printf("last reading time == %s\n", curr_dir->lastReadTime);
    printf("no. of children == %d\n", curr_dir->n_children);
    printf("parent directory == %d\n", curr_dir->parent_directory);
    printf("size of file == %d\n", curr_dir->size);
    printf("type of file == %c\n", curr_dir->type);
    printf("\n");
    
    for (int i=0; i< curr_dir->n_children; i++){
        dir_entry* curr_chld = &(vd->disk->d_table[curr_dir->children[i]]);
        if ((curr_chld->name!='\0' && curr_chld->name!= curr_dir->name) && (curr_chld->type=='d' || curr_chld->type=='f')){
                printf("\n");
		printf("child no. %d\n", i);
                printf("current dir/file == %s\n",curr_chld->name);
                printf("first block in FAT table == %d\n",curr_chld->first_fat_block);
                printf("creation time == %s\n", curr_chld->creationTime);
                printf("last writing time == %s\n", curr_chld->lastWriteTime);
    		printf("last reading time == %s\n", curr_chld->lastReadTime);
                printf("no. of children == %d\n", curr_chld->n_children);
                printf("parent directory == %d\n", curr_chld->parent_directory);
                printf("size of file == %d\n", curr_chld->size);
                printf("type of file == %c\n", curr_chld->type);
                memmove(res[i], curr_chld, sizeof(dir_entry));
                printf("memmove no.%d complete!\n", i);
                printf("\n");
           }
    }
    return res;
  }
}

  int FAT_write(FileHandle* fd, const char* buf, size_t size){
    if (fd->mode == READING_MODE){
        printf("Reading only mode!\n");
        return -1;
    }
    else{
    
    int written_bytes = 0;
    int must_write = 0;
    int repeated_blocks = 0;
    virtual_disk* vd = fd->vd;
    int curr_fat_index = fd->block_index;
    data_block* data = getDataBlock(fd, &curr_fat_index);
    if (fd->pos == BLOCK_SIZE +1){
        data = searchFreeDataBlock(fd, &curr_fat_index);
        if (data == NULL){
            printf("Not enough space on the virtual disk!\n");
            return -1;
        }
    }

    
    while (written_bytes < size){
        must_write = size - written_bytes;
        memmove(data + fd->pos, buf, must_write);
        printf("prev rel pos == %d\n", fd->pos);
        fd->pos = fd->pos + must_write;
        printf("curr rel pos == %d\n", fd->pos);
        buf = buf + must_write;
        printf("prev written bytes == %d\n", written_bytes);
        written_bytes = written_bytes + must_write;
        printf("curr written bytes == %d\n", written_bytes);
        printf("prev file size == %d\n", vd->disk->d_table[fd->dir_entry].size);
        vd->disk->d_table[fd->dir_entry].size = vd->disk->d_table[fd->dir_entry].size + (written_bytes*sizeof(char));
        printf("curr file size == %d\n", vd->disk->d_table[fd->dir_entry].size);
        if (fd->pos >= BLOCK_SIZE){
            fd->pos = fd->pos % BLOCK_SIZE;
            repeated_blocks++;
            data =  getFreeDataBlock(vd, curr_fat_index);
            if ((data == NULL)){
                fd->pos = BLOCK_SIZE+1;
                break;
            }

        }

    }
    fd->block_index = fd->block_index+repeated_blocks;
    printf("\n");
    printf("%s\n", data+fd->pos);
    vd->disk->d_table[fd->dir_entry].lastWriteTime = getTime(); 
   return written_bytes;
   }

  }

int FAT_read(FileHandle* fd, void* buf, size_t size){
if (fd->mode == WRITING_MODE){
        printf("Writing only mode!\n");
        return -1;
    }
    else if (fd->vd->disk->d_table[fd->dir_entry].size == 0){
        printf("File is empty! Cannot perform reading!\n");
        return -1;
    }
else{
    int read_bytes = 0;
    int must_read = 0;
    int repeated_blocks = 0;
    int curr_fat_index = fd->block_index;
    virtual_disk* vd = fd->vd;
    data_block* data = getDataBlock(fd, &curr_fat_index);
    int f_size = vd->disk->d_table[fd->dir_entry].size;
    if (fd->pos == BLOCK_SIZE +1){
        data = searchFreeDataBlock(fd, &curr_fat_index);
        if (data == NULL){
            printf("Not enough space on the virtual disk!\n");
            return -1;
        }
    }
    while (read_bytes < size){
            must_read = size - read_bytes;
        printf("must read == %d\n", must_read);
        memmove(buf, data+fd->pos, must_read);
        printf("prev rel pos == %d\n", fd->pos);
        fd->pos = fd->pos + must_read;
        printf("curr rel pos == %d\n", fd->pos);
        buf = buf + must_read;
        printf("prev read bytes == %d\n", read_bytes);
        read_bytes = read_bytes + must_read;
        printf("curr read bytes == %d\n", read_bytes);
        if (fd->pos == BLOCK_SIZE){
            fd->pos = 0;
            repeated_blocks++;
            if ((searchFreeDataBlock(fd, curr_fat_index)) == NULL){
                fd->pos = BLOCK_SIZE + 1;
                break;
            }
        }

    fd->block_index = fd->block_index + repeated_blocks;

    printf("\n");
    vd->disk->d_table[fd->dir_entry].lastReadTime = getTime();
    return read_bytes;
    }
  }
}

int FAT_seek(FileHandle* fd, int offset, int whence){
    int res;
    if (whence > SEEK_END){
        return -1;
    }
    if (whence == SEEK_SET){
        if (offset<0){
            return -1;
        }
        res=offset;
    }
    else if (whence == SEEK_CUR){
        res = ((fd->block_index * BLOCK_SIZE) + fd->pos) + offset;
        if (res>(fd->vd)->disk->d_table[fd->dir_entry].size){
            return -1;
        }
    }
    else if (whence == SEEK_END){
        if (offset>0){
            return -1;
        }
        res = (fd->vd)->disk->d_table[fd->dir_entry].size;
        res = res + offset;
        if (res > (fd->vd)->disk->d_table[fd->dir_entry].size){
            return -1;
        }
    }
    printf("prev block_index == %d\n", fd->block_index);
    printf("prev pos == %d\n", fd->pos);
    fd->block_index = res / BLOCK_SIZE;
    fd->pos = res % BLOCK_SIZE;
    printf("curr block_index == %d\n", fd->block_index);
    printf("curr pos == %d\n", fd->pos);
    return 0;
}
