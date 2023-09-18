#include "fat.h"

int main(){
    char read_buff[10] = {0};
    virtual_disk* vd = startFAT("provaFileSystemFat");
    printf("startFAT completed!\n");
    printf("\n");
    printf("\n");
    FileHandle* file = createFile(vd, "fileditesto.txt", 'x');
    printf("createFile completed!\n");
    printf("\n");
    printf("\n");
    FileHandle* file2 = createFile(vd, "filediprova.txt", 'x');
    printf("createFile completed!\n");
    printf("\n");
    printf("\n");

    FileHandle* file3 = createFile(vd, "file.txt", 'x');
    printf("createFile completed!\n");
    printf("\n");
    printf("\n");

    int res= createDir(vd, "dirDiProva1");   
    printf("createDir completed! Esito == %d\n", res);
    printf("\n");
    printf("\n");

    res= createDir(vd, "dirDiProva2");   
    printf("createDir completed! Esito == %d\n", res);
    printf("\n");
    printf("\n");


    res = eraseDir(vd, "dirDiProva2");
    printf("eraseDir completed! Esito == %d\n", res);
    printf("\n");
    printf("\n");


    
    printf("\n");
    int written = FAT_write(file, "tottiGol", sizeof("tottiGol"));
    printf("\n");
    printf("\n");
    printf("FAT_write completed! Caratteri scritti == %d\n", written);
    written = FAT_write(file, "ForzaRoma!", sizeof("ForzaRoma!"));
    printf("\n");
    printf("\n");
    printf("FAT_write completed! Caratteri scritti == %d\n", written);
for (int i=0; i<MAX_BLOCKS; i++){
       if (vd->disk->f_table[i]!=-1){
         printf("fat table ==%d\n", vd->disk->f_table[i]);
     }
  }

    for (int i=0; i<MAX_ENTRIES; i++){
       if (vd->disk->d_table[i].name!=NULL){
         printf("dir table pos no. %d == %s\n", i, vd->disk->d_table[i].name);
       }
  }
    int ris = FAT_seek(file, 1, SEEK_SET);
    printf("\n");
    printf("\n");
    printf("FAT_seek completed! Esito == %d, nuova posizione == %d\n", ris, file->pos);
    printf("\n");
    int read = FAT_read(file, read_buff, sizeof(read_buff));
    printf("\n");
    printf("\n");
    printf("FAT_read completed! Caratteri letti == %d\n", read);
    printf("\n");
    dir_array* test=listDir(vd);
    res=eraseFile(vd, "fileditesto.txt");
    printf("eraseFile completed! Esito ==%d\n", res);
    printf("\n");
    printf("\n");
    
    res=changeDir(vd, "dirDiProva1");
    printf("changeDir completed! Esito ==%d\n", res);
    printf("\n");
    printf("\n");
    res=changeDir(vd, "/");
    printf("changeDir completed! Esito ==%d\n", res);

    printf("working dir == %d\n", vd->curr_dir);
    printf("\n");
    printf("\n");

    res = eraseFile(vd, "filediprova.txt");
    printf("eraseFile completed! Esito ==%d\n", res);
    printf("\n");
    printf("\n");

    res = eraseFile(vd, "file.txt");
    printf("eraseFile completed! Esito ==%d\n", res);
    printf("\n");
    printf("\n");

    res = eraseDir(vd, "dirDiProva1");
    printf("eraseDir completed! Esito == %d\n", res);
    printf("\n");
    printf("\n");



    printf("\n");
    res=killFAT(vd);
    printf("\n");
    printf("\n");
    printf("killFAT completed! Esito ==%d\n", res);
    return 0;
}
