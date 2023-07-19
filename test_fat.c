#include "fat.h"

int main(){
    char read_buff[10];
    virtual_disk* vd = startFAT("provaFileSystemFat");
    printf("startFAT completed!\n");
    printf("\n");
    printf("\n");
    listDir(vd);
    FileHandle* file = createFile(vd, "fileditesto.txt");
    printf("createFile completed!\n");
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
    
    
    int ris = FAT_seek(file, 1, SEEK_SET);
    printf("\n");
    printf("\n");
    printf("FAT_seek completed! Esito == %d\n", ris);
    printf("\n");
    int read = FAT_read(file, read_buff, sizeof(read_buff));
    printf("\n");
    printf("\n");
    printf("FAT_read completed! Caratteri letti == %d\n", read);
    printf("\n");
    dir_array* test =listDir(vd);
    int res= createDir(vd, "dirDiProva1");   
    printf("createDir completed! Esito == %d\n", res);
    printf("\n");
    printf("\n");
    listDir(vd);
    res=changeDir(vd, "dirDiProva1");
    printf("changeDir completed! Esito ==%d\n", res);
    printf("\n");
    printf("\n");
    res=eraseFile(vd, "file12432.txt");
    printf("eraseFile completed! Esito ==%d\n", res);
    printf("\n");
    printf("\n");
    res=eraseFile(vd, "mitboll1");
    printf("eraseFile completed! Esito ==%d\n", res);
    printf("\n");
    printf("\n");
    res=changeDir(vd, "/");
    printf("changeDir completed! Esito ==%d\n", res);
    printf("\n");
    printf("\n");
    test=listDir(vd);
    printf("\n");
    res=killFAT(vd);
    printf("\n");
    printf("\n");
    printf("killFAT completed! Esito ==%d\n", res);
    return 0;
}