#include "fat.h"
int main(){
	char buf[5000] = {0};
	virtual_disk* vd = startFAT("provaFileSystemFat");
	printf("startFAT completed!\n");
    	printf("\n");
        FileHandle* file = createFile(vd, "fileditesto.txt", 'x');
    	printf("\n");
    	printf("\n");

    	FileHandle* file2 = createFile(vd, "file.txt", 'x');
    	printf("\n");
    	printf("\n");

	FileHandle* file3 = createFile(vd, "fileditesto.txt", 'x');
    	printf("\n");
    	printf("\n");


      
        for (int i=0; i<10; i++){
         printf("fat table pos. no.%d == %d\n", i, vd->disk->f_table[i]);
  	}

        for (int i=0; i<10; i++){
       if (vd->disk->d_table[i].name!=NULL){
         printf("dir table pos no.%d == %s\n", i, vd->disk->d_table[i].name);
       }
  }
	int res= createDir(vd, "directorydiProva");
        printf("\n");
        printf("\n");
        res = createDir(vd, "directoryDiProva2");
        printf("\n");
        printf("\n");
	res = createDir(vd, "directoryDiProva2");
        printf("\n");
        printf("\n");
        for (int i=0; i<10; i++){
         printf("fat table pos. no.%d == %d\n", i, vd->disk->f_table[i]);
  	}

        for (int i=0; i<10; i++){
       if (vd->disk->d_table[i].name!=NULL){
         printf("dir table pos no.%d == %s\n", i, vd->disk->d_table[i].name);
       	}
  	}

	res = eraseFile(vd, "file.txt", file2);
        eraseFileAux(file);
        printf("\n");
        printf("\n");
        printf("Esito eraseFile == %d\n", res);
	res = eraseFile(vd, "fileditesto.txt", file);
        printf("\n");
        printf("\n");
        printf("Esito eraseFile == %d\n", res);
	res = eraseFile(vd, "fileditesto.txt", file3);
        printf("\n");
        printf("\n");
        printf("Esito eraseFile == %d\n", res);
	res = eraseFile(vd, "fileditesto.txt", file);
        printf("\n");
        printf("\n");
        printf("Esito eraseFile == %d\n", res);

for (int i=0; i<10; i++){
         printf("fat table pos. no.%d == %d\n", i, vd->disk->f_table[i]);
  	}

        for (int i=0; i<10; i++){
       if (vd->disk->d_table[i].name!=NULL){
         printf("dir table pos no.%d == %s\n", i, vd->disk->d_table[i].name);
       	}
  	}

	res = eraseDir(vd, "directoryDiProva2");
        printf("Esito eraseDir == %d\n", res);
	printf("\n");
        printf("\n");
        
	res = eraseDir(vd, "directoryDiProva2");
        printf("Esito eraseDir == %d\n", res);
	printf("\n");
        printf("\n");
	for (int i=0; i<10; i++){
         printf("fat table pos. no.%d == %d\n", i, vd->disk->f_table[i]);
  	}

        for (int i=0; i<10; i++){
       if (vd->disk->d_table[i].name!=NULL){
         printf("dir table pos no.%d == %s\n", i, vd->disk->d_table[i].name);
       	}
  	}

	res=changeDir(vd, "directorydiProva");
        printf("Esito changeDir == %d\n", res);
	printf("\n");
        printf("\n");
        res=changeDir(vd, "..");
	printf("Esito changeDir == %d\n", res);
	printf("\n");
        printf("\n");
	printf("Esito changeDir == %d\n", res);
	printf("\n");
        printf("\n");
	FileHandle* file4 = createFile(vd, "hoSete.txt", 'x');
    	printf("\n");
    	printf("\n");
	FileHandle* file5 = createFile(vd, "hoTantoFame.txt", 'x');
    	printf("\n");
    	printf("\n");
    	FileHandle* file6 = createFile(vd, "hoSonno.txt", 'x');
    	printf("\n");
    	printf("\n");

		for (int i=0; i<10; i++){
         printf("fat table pos. no.%d == %d\n", i, vd->disk->f_table[i]);
  	}

        for (int i=0; i<10; i++){
       if (vd->disk->d_table[i].name!=NULL){
         printf("dir table pos no.%d == %s\n", i, vd->disk->d_table[i].name);
       	}
  	}

	dir_array* list= listDir(vd);
	printf("\n");
        printf("\n");

		for (int i=0; i<10; i++){
         printf("fat table pos. no.%d == %d\n", i, vd->disk->f_table[i]);
  	}

        for (int i=0; i<10; i++){
       if (vd->disk->d_table[i].name!=NULL){
         printf("dir table pos no.%d == %s\n", i, vd->disk->d_table[i].name);
       	}
  	}


	res= FAT_write(file4, "TestDiScrittura", sizeof("TestDiScrittura"));

	res = FAT_write(file4, "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum. Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium doloremque laudantium, totam rem aperiam, eaque ipsa quae ab illo inventore veritatis et quasi architecto beatae vitae dicta sunt explicabo. Nemo enim ipsam voluptatem quia voluptas sit aspernatur aut odit aut fugit, sed quia consequuntur magni dolores eos qui ratione voluptatem sequi nesciunt. Neque porro quisquam est, qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit, sed quia non numquam eius modi tempora incidunt ut labore et dolore magnam aliquam quaerat voluptatem. Ut enim ad minima veniam, quis nostrum exercitationem ullam corporis suscipit laboriosam, nisi ut aliquid ex ea commodi consequatur? Quis autem vel eum iure reprehenderit qui in ea voluptate velit esse quam nihil molestiae consequatur, vel illum qui dolorem eum fugiat quo voluptas nulla pariatur?", sizeof ("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum. Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium doloremque laudantium, totam rem aperiam, eaque ipsa quae ab illo inventore veritatis et quasi architecto beatae vitae dicta sunt explicabo. Nemo enim ipsam voluptatem quia voluptas sit aspernatur aut odit aut fugit, sed quia consequuntur magni dolores eos qui ratione voluptatem sequi nesciunt. Neque porro quisquam est, qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit, sed quia non numquam eius modi tempora incidunt ut labore et dolore magnam aliquam quaerat voluptatem. Ut enim ad minima veniam, quis nostrum exercitationem ullam corporis suscipit laboriosam, nisi ut aliquid ex ea commodi consequatur? Quis autem vel eum iure reprehenderit qui in ea voluptate velit esse quam nihil molestiae consequatur, vel illum qui dolorem eum fugiat quo voluptas nulla pariatur?"));



	res = FAT_read(file4, buf, sizeof(buf));
	
	printf("\n");
        printf("\n");
	res = FAT_seek(file4, -250, SEEK_END);
        res = FAT_seek(file4, 19, SEEK_CUR);
        res = FAT_seek(file4, -469, SEEK_CUR);
        res = FAT_seek(file4, 102, SEEK_CUR);
	res = FAT_read(file4, buf, sizeof(buf));
	printf("\n");
        printf("\n");
	





	res = killFAT(vd);
        printf("Esito killFAT == %d\n", res);

        
     
    	return 0;
}
