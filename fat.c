#include "fat.h"
#define root_dir 0
#include <time.h>
int pos_fat = 1;

char* getTime(){
    time_t mytime = time(NULL);
    char * time_str = ctime(&mytime);
    time_str[strlen(time_str)-1] = '\0';
    return time_str;
}

void killFile(FileHandle* file){
    free(file);
}

static void addChild(dir_entry* parent, int child){
    
    for (int i=0; i<MAX_CHILDREN; i++){
        int* child_i = &(parent->children[i]);
        printf("child == %d\n", child);
        printf("child_i == %d\n", *child_i);
        if (*child_i == NULL ){
            printf("Free space detected! Adding child...\n");
            *child_i = child;
            parent->n_children++;
            parent->lastWriteTime = getTime();
            parent->lastReadTime = getTime();
            break;
        }
    }
}

static void removeChild(dir_entry* parent, int child){
    printf("Children detected! Proceeding to elimination...\n");
    for (int i=0; i<MAX_CHILDREN; i++){
        int* child_i = &(parent->children[i]);
        printf("Child no. %d == %d\n", i, *(child_i));
        if (*child_i == child){
            printf("Target acquired!\n");
            memset(&(child_i), NULL, sizeof(&(child_i)));
	     parent->n_children--;
             parent->lastWriteTime = getTime();
             parent->lastReadTime = getTime();
            break;
        }
        if (*child_i == 0){
	     parent->n_children--;
             parent->lastWriteTime = getTime();
             parent->lastReadTime = getTime();
            break;
        }
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


static int createKnownEntry(virtual_disk* vd,const char* name, int id, char type, int fatPos, int freeIndex){
	int freeBlock;
        freeBlock= findFreeFATBlock(vd);
        printf("%d\n", freeBlock);
        if (freeBlock == -1){
            return -1;
        }
        
        printf("curr. fat table block no. %d == %d\n", vd->disk->f_table[freeIndex], fatPos);
        dir_entry* entry= (&(vd->disk->d_table[freeIndex]));
        dir_entry* known_entry= (&(vd->disk->d_table[id]));
        strcpy(&(entry->name[0]), name);
        printf("entry->name == %s\n", entry->name);
        entry->first_fat_block = fatPos;
        printf("entry->first_fat_block == %d\n", entry->first_fat_block);
	known_entry->next_fat_block = entry->first_fat_block;
        printf("known_entry->next_fat_block == %d\n", known_entry->next_fat_block);
        entry->next_fat_block = END_OF_CHAIN;
	printf("entry->next_fat_block == %d\n", entry->next_fat_block);
        entry->size = known_entry->size;
        printf("entry->size == %d\n", entry->size);
        entry->type = type;
        printf("entry->type == %c\n", entry->type);
        entry->creationTime = known_entry->creationTime;
        if (type == DIRECTORY){
           entry->lastWriteTime = getTime();
           printf("\n");
           entry->lastReadTime = getTime();
	}
        if (type == FILE){
           entry->lastWriteTime = known_entry->lastWriteTime;
           entry->lastReadTime = known_entry->lastReadTime;
	}
        printf("entry->lastWriteTime == %s\n", entry->lastWriteTime);
        printf("entry->lastReadTime == %s\n", entry->lastReadTime); 	
        entry->parent_directory = vd->curr_dir;
        printf("entry->parent_directory == %d\n", entry->parent_directory);
        vd->disk->f_table[fatPos] = freeIndex;
	vd->disk->f_table[freeIndex] = entry->next_fat_block;
        addChild((&(vd->disk->d_table[entry->parent_directory])), id);
        if (type == DIRECTORY){
            entry->n_children = 0;
            printf("entry->n_children == %d\n", entry->n_children);
            memset(entry->children, 0, sizeof(entry->children));
        }
	printf("createEntry completata!\n");
        return 0;
    }


static int createEntry(virtual_disk* vd,const char* name, int id, char type){
    int freeBlock;
        freeBlock= findFreeFATBlock(vd);
        if (freeBlock == -1){
            return -1;
        }

        dir_entry* entry= (&(vd->disk->d_table[id]));
        strcpy(&entry->name[0], name);
        printf("entry->name == %s\n", entry->name);
        entry->first_fat_block = freeBlock;
        printf("entry->first_fat_block == %d\n", entry->first_fat_block);
	entry->next_fat_block = END_OF_CHAIN;
	printf("entry->next_fat_block == %d\n", entry->next_fat_block);
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
           entry->lastWriteTime = getTime();
           entry->lastReadTime = getTime();
	}
        printf("entry->lastWriteTime == %s\n", entry->lastWriteTime);
        printf("entry->lastReadTime == %s\n", entry->lastReadTime); 	
        entry->parent_directory = vd->curr_dir;
        printf("entry->parent_directory == %d\n", entry->parent_directory);
	vd->disk->f_table[freeBlock] = entry->next_fat_block;
        printf("Must add child to his parent!\n");
        addChild((&(vd->disk->d_table[entry->parent_directory])), id);
        if (type == DIRECTORY){
            entry->n_children = 0;
            printf("entry->n_children == %d\n", entry->n_children);
            memset(entry->children, 0, sizeof(entry->children));
        }
        printf("createEntry completata!\n");
        return 0;
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
    entry->first_fat_block = 0;
    entry->next_fat_block = END_OF_CHAIN;
    entry->n_children = 0;
    entry->parent_directory=-1;
    memset(entry->children, 0, sizeof(entry->children));
    printf("setup ok!\n");
    vd->size = sizeof(fat_table) + sizeof(data_block) * MAX_BLOCKS;
    vd->curr_dir = 0;
    vd->disk->f_table[entry->first_fat_block] = END_OF_CHAIN;
    printf("vd->size == %d\n", vd->size);
    printf("vd->curr_dir == %d\n", vd->curr_dir);
    printf("vd->fd == %d\n", vd->fd);
    printf("vd->disk == %p\n", vd->disk);
    return vd;
}

int killFAT(virtual_disk* vd){
	int res = memset(&(vd->disk->f_table), UNUSED, sizeof(fat_table));
   if (res == -1){
	printf("Cannot clear FAT table!\n");
        return -1;
	}
    res = memset(&(vd->disk->d_table), NULL, sizeof(dir_table));
   if (res == -1){
	printf("Cannot clear dir table!\n");
        return -1;
	}

    res=memset(&(vd->disk->data), NULL, sizeof(vd->disk->data));
   if (res == -1){
	printf("Cannot clear data table!\n");
        return -1;
	}

    
   res= munmap(vd->disk, vd->size);
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
        printf("File individuato!\n");
        createKnownEntry(vd, name, file_index, FILE, vd->disk->d_table[file_index].first_fat_block, free_index);
        printf("Setto i parametri del FileHandle...\n");
        file->block_index = 0;
        file->pos = 0;
        file->dir_entry = file_index;
        file->mode = mode;
        file->vd = vd;
        printf("file->block_index == %d\n", file->block_index);
        printf("file->pos == %d\n", file->pos);
        printf("file->dir_entry == %d\n", file->dir_entry);
        printf("file->disk == %p\n", file->vd);
        printf("file->mode == %c\n", file->mode);
        printf("CreateFile completata!\n");
        return file;
        
    }
    else{
        int status = createEntry(vd, name, free_index, FILE);
        if (status== -1){
            free(file);
            printf("Not enough space on the disk!\n");
            return NULL;
        }
        else if (status==0){
        printf("Setto i parametri del FileHandle...\n");
        file->block_index = 0;
        file->pos = 0;
        file->dir_entry = free_index;
        file->mode = mode;
        file->vd = vd;
        printf("file->block_index == %d\n", file->block_index);
        printf("file->pos == %d\n", file->pos);
        printf("file->dir_entry == %d\n", file->dir_entry);
        printf("file->disk == %p\n", file->vd);
        printf("file->mode == %c\n", file->mode);
        printf("CreateFile completata!\n");
        return file;
}

}
}

int eraseFile(virtual_disk* vd, const char* name, FileHandle* file){
    if (findIndex(vd, name, FILE) == -1){
      printf("No such file on this system!\n");
      return -1;
     }
    else{
    while (findIndex(vd, name, FILE) != -1){
    int index = findIndex(vd, name, FILE);
    if (index == file->dir_entry){
    printf("%d\n", index);
    dir_entry* entry = &(vd->disk->d_table[index]);
    int curr_e = entry->first_fat_block;
    for (int i=0; i<MAX_BLOCKS; i++){
            int new_e = vd->disk->f_table[i];
            if (curr_e == new_e){
	 	vd->disk->f_table[i] = UNUSED;
	        
        }
}
    removeChild(&(vd->disk->d_table[entry->parent_directory]), index);
    vd->disk->f_table[index] = UNUSED;
    memset(entry, 0, sizeof(dir_entry));
	}

if (index == file->dir_entry){
	eraseFileAux(file);
    printf("File deleted successfully!\n");
return 0;
}
else{
  printf("File inserted not correct!\n");
  return -1;
}
}
}
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

void eraseFileAux(FileHandle* file){
killFile(file);
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
        printf("Directory already exists with this name!\n");
        return 0;
    }
    return createEntry(vd, name, free_index, DIRECTORY);
}

int eraseDir(virtual_disk* vd, const char* name){
    if (findIndex(vd, name, DIRECTORY) == -1){
      printf("No such directory on this system!\n");
      return -1;
     }
    else{
	while (findIndex(vd, name, DIRECTORY)!=-1){
    int index= findIndex(vd, name, DIRECTORY);
    if (index == -1){
        printf("Directory not found!\n");
        return -1;
    }
    dir_entry* entry = &(vd->disk->d_table[index]);
    int curr_e = entry->first_fat_block;
	for (int i=0; i<MAX_BLOCKS; i++){
            int new_e = vd->disk->f_table[i];
            if (curr_e == new_e){
	 	vd->disk->f_table[i] = UNUSED;
        }
}
	if (entry->n_children > 0){
        printf("Entry is a file!\n");
        return -1;
    }

    vd->disk->f_table[index] = UNUSED;
    removeChild(&(vd->disk->d_table[entry->parent_directory]), index);
    memset(entry, 0, sizeof(dir_entry));
}
}
	printf("Directory deleted successfully!\n");
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
    dir_array* res = (dir_array*)malloc(MAX_ENTRIES*sizeof(dir_entry*));
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

    
    for (int i=0; i< curr_dir->n_children+1; i++){
        dir_entry* curr_elem = &(vd->disk->d_table[i]);
        printf("parent directory == %d,   first fat block ==%d\n",  curr_elem->parent_directory, curr_dir->first_fat_block);
        if ((curr_elem->parent_directory == curr_dir->first_fat_block) && curr_elem->name!='\0'){
        
                printf("\n");
		printf("child no. %d\n", i);
                printf("current dir/file == %s\n",curr_elem->name);
                printf("first block in FAT table == %d\n",curr_elem->first_fat_block);
                printf("creation time == %s\n", curr_elem->creationTime);
                printf("last writing time == %s\n", curr_elem->lastWriteTime);
    		printf("last reading time == %s\n", curr_elem->lastReadTime);
                printf("no. of children == %d\n", curr_elem->n_children);
                printf("parent directory == %d\n", curr_elem->parent_directory);
                printf("size of file == %d\n", curr_elem->size);
                printf("type of file == %c\n", curr_elem->type);
                memmove(res[i], curr_elem, sizeof(curr_elem));
                printf("memmove no.%d complete!\n", i);
                printf("\n");
	}
    }
    return res;
  }
}

data_block* getFreeDataBlock(virtual_disk* vd, int fat_index){
    int next_index = vd->disk->f_table[fat_index];
    if (next_index != END_OF_CHAIN){
        fat_index = next_index;
        return &(vd->disk->data[next_index]);
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

int FAT_write(FileHandle* fd, const char* buf, size_t size){
    if (fd->mode == READING_MODE){
        printf("Reading only mode!\n");
        return -1;
    }

    else{
    int written_bytes = 0;
    int must_write = 0;
    int repeated_blocks = 0;
    int curr_data_index = fd->block_index;
    printf("curr_data_index == %d\n", curr_data_index);
    int pos = (fd->block_index * BLOCK_SIZE) + fd->pos;
    printf("pos == %d\n", pos);
    virtual_disk* vd = fd->vd;
    
    data_block* data = getDataBlock(fd, &curr_data_index);
    printf("DataBlock individuato: %p\n", data);
    if (fd->pos == BLOCK_SIZE){
        data = searchFreeDataBlock(fd, &curr_data_index);
        if (data == NULL){
            printf("Not enough space on the virtual disk!\n");
            return -1;
        }
	printf("DataBlock individuato: %p\n", data);
    }

    
    while (written_bytes < size){
        must_write = size - written_bytes;
	printf("Need to write %d bytes... Let's go!\n", must_write);
        memmove(data + fd->pos, buf, must_write);
        printf("prev rel pos == %d\n", fd->pos);
        fd->pos = fd->pos + must_write;
        printf("curr rel pos == %d\n", fd->pos);
        printf("prev pos == %d\n", pos);
        pos = pos + must_write;
        printf("curr pos == %d\n", pos);
        buf = buf + must_write;
        printf("prev written bytes == %d\n", written_bytes);
        written_bytes = written_bytes + must_write;
        printf("curr written bytes == %d\n", written_bytes);
        printf("prev file size == %d\n", vd->disk->d_table[fd->dir_entry].size);
        vd->disk->d_table[fd->dir_entry].size = vd->disk->d_table[fd->dir_entry].size + (written_bytes*sizeof(char));
        printf("curr file size == %d\n", vd->disk->d_table[fd->dir_entry].size);

	if (pos >= BLOCK_SIZE){
	    printf("Oh no! We've reached max capacity! No problem, here we go!\n");
            pos = pos % BLOCK_SIZE;
            repeated_blocks++;
            data =  getFreeDataBlock(vd, curr_data_index);
	    printf("DataBlock individuato: %p\n", data);    
            if ((data == NULL)){
                fd->pos = BLOCK_SIZE+1;
                break;
            }
        }
	

    }
    fd->pos = pos;
    fd->block_index = fd->block_index+repeated_blocks;
    printf("\n");
    printf("fd->pos == %d\n", fd->pos);
    printf("repeated_blocks == %d\n", repeated_blocks);
    vd->disk->d_table[fd->dir_entry].lastWriteTime = getTime(); 
    printf("FAT_write completata! Sono stati scritti %d bytes!\n", written_bytes);
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
    int curr_data_index = fd->block_index;
    printf("curr_data_index == %d\n", curr_data_index);
    int pos = (fd->block_index * BLOCK_SIZE) + fd->pos;

    virtual_disk* vd = fd->vd;
    int f_size = vd->disk->d_table[fd->dir_entry].size;

    data_block* data = getDataBlock(fd, &curr_data_index);
    printf("DataBlock individuato: %p\n", data);
    if (fd->pos == BLOCK_SIZE +1){
	printf("sono qui?\n");
        data = searchFreeDataBlock(fd, &curr_data_index);
        if (data == NULL){
            printf("Not enough space on the virtual disk!\n");
            return -1;
        }
	printf("DataBlock individuato: %p\n", data);
    }
	    printf("fd->pos == %d\n", fd->pos);
	    printf("pos == %d\n", pos);
	    printf("f_size == %d\n", f_size);
	if ((pos+fd->pos)> f_size){
	size= f_size-pos;
	}

	printf("size == %d\n", size);

	if (size==0){
	printf("There is nothing to read!\n");
	return -1;
	}


    while (read_bytes < size){
            must_read = f_size - pos;
        printf("must read == %d\n", must_read);
        memmove(buf, (data+fd->pos), must_read);
        printf("prev rel pos == %d\n", fd->pos);
        fd->pos = fd->pos + must_read;
        printf("curr rel pos == %d\n", fd->pos);
	printf("prev pos == %d\n", pos);
        pos = pos + must_read;
        printf("curr pos == %d\n", pos);
        buf = buf + must_read;
        printf("prev read bytes == %d\n", read_bytes);
        read_bytes = read_bytes + must_read;
        printf("curr read bytes == %d\n", read_bytes);
        if (pos >= BLOCK_SIZE){
	    printf("Oh no! We've reached max capacity! No problem, here we go!\n");
            pos = 0;
            repeated_blocks++;
            data = searchFreeDataBlock(fd, curr_data_index);
	    printf("DataBlock individuato: %p\n", data);
            if ((data) == NULL){
                fd->pos = BLOCK_SIZE + 1;
                break;
            }
        }
    printf("pos == %d\n", pos);
    fd->pos = pos;
    printf("fd->pos == %d\n", fd->pos);
    fd->block_index = fd->block_index + repeated_blocks;
    printf("fd->block_index == %d\n", fd->block_index);

    printf("\n");
    vd->disk->d_table[fd->dir_entry].lastReadTime = getTime();
    printf("FAT_read completata! Sono stati letti %d bytes!\n", read_bytes);
    return read_bytes;
    }
  }
}

int FAT_seek(FileHandle* fd, int offset, int whence){
    int res;
    if (whence > SEEK_END){
	printf("Position is too far!\n");
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























