# ProgettoSO(definitivo)
 Implementazione File System

L'idea di base per l'implementazione di questo file system prevede che l'immagine di memoria del disco contenga tre elementi specifici:
   1. Un'array di data_block, denominato data di dimensione MAX_BLOCKS, dove ciascun data_block contiene un vettore di char di dimensione BLOCK_SIZE.

   2. Un'array di dir_entry di dimensione MAX_ENTRIES dove ciascun dir_entry rappresenta di fatto un FCB, contenente quindi tutti gli elementi che caratterizzano un file (nome, dimensione, tipologia ecc...).

   3. Un'array di fat_entry, rappresentati per semplicità con un intero, il quale serve per tenere traccia delle aree del disco disponibili e quelle usate per file/directory-


In seguito, si passa ad implementare una struttura dati essenziale, la quale permette di operare sull'immagine di memoria del disco, il disco virtuale e il FileHandle previsto dalle specifiche del progetto.

Infine, è stata implementata la struttura dir_array, la quale di fatto contiene un'array di dir_entry, necessaria per la funzione listDir.


Nella funzione startFAT, viene inizializzata la struttura dati principale, ovvero il disco virtuale: tramite tale funzione, viene aperto il file descriptor e adattato alle dimensioni del disco virtuale (open + ftruncate). In seguito, viene inizializzato il buffer di memoria contenente il disco e anche la tabella fat, la quale contiene di fatto tutti indici di tipo UNUSED. Infine, viene eseguito il setup della cartella root, la quale viene anche posta come directory corrente.


Nella funzione killFAT, vengono eseguite le funzioni inverse di quelle presenti nella startFAT: viene deallocata la zona di memoria per il disco (munmap) e chiuso il descrittore.

Nella funzione createFile, vengono inizializzate due indici, i quali rispettivamente rappresentano la posizione in cui si trova il file all'interno della tabella delle dir_entry e la prima locazione libera sempre all'interno di essa. In base a tali valori, si eseguono diverse operazioni:
     1. Se entrambi sono uguali a -1, ciò significa che il file non è stato trovato e che non è presente una locazione libera all'interno della tabella delle dir_entry, per cui è necessario stampare un errore.

     2. Se la posizione è diversa da -1, ciò significa che il file è presente nella tabella e deve essere creata un FileHandle, in cui la dir_entry è uguale alla posizione. Altrimenti, viene sempre creato un FileHandle, ma la dir_entry è uguale alla prima locazione libera.


Nella funzione createDir, viene eseguito il controllo sulla posizione della directory, se esistente, e l'individuazione della prima locazione libera all'interno della tabella delle dir_entry. In base a tali valori, si eseguono differenti operazioni:
     1. Se entrambi sono uguali a -1, ciò significa che la directory non è stato trovata e che non è presente una locazione libera all'interno della tabella delle dir_entry, per cui è necessario stampare un errore.

     2. Se il primo è diversa da -1, ciò significa che la directory esiste, per cui non è necessario creare una entry per essa. Altrimenti si ritorna l'esito della funzione createEntry, deputata alla inizializzazione della dir_entry.

Nella funzione eraseFile, si segue l'idea di base della createFile, ovvero si ricerca se il file da eliminare risulta presente nella tabella delle dir_entry: in caso tale controllo comporti esito negativo, si ritorna errore. Altrimenti, è necessario andare a pulire le sezioni della fat table occupate dal file, prima di deallocare la struttura dati utilizzata. Nel caso in cui il file non abbia come directory padre la root directory, è necessario pulire anche la sezione dei figli relativa al padre del file.

Nella funzione eraseDir, si segue il procedimento descritto dalla eraseFile, ad eccezione della pulizia della FAT table. 