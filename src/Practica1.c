#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

#include "red-black-tree.h"

#define MAXCHAR  200
#define MAXHASHSIZE 500
#define N 1000

#define GNUPLOT "gnuplot -persist"

int hashIndex(char *str,int seed,int hashSize);
void initHashList(List *list,char *origin, char *destiny, int dayOfWeek, int delay);
char **readNLines(FILE * fp,int numberOfLines);
List ** generateHash(char **str,int maxHashSize,int nLines);
void addHashToTree(List **hash,int maxHashSize, RBTree *tree);
void mergeLists(RBData *treeData, List* hashList);
void deleteHash(List **hash,int maxHashSize);
ListData * copyListData(ListData *list);
List * copyList(List *list);
void freeLines(char **strVec,int lineNumber);
RBTree * createTree(char *filename);
List * readList(FILE *fp);
RBTree * readTree(char * filename);
void printOnGnuPlot();
void writeGnuPlotData(RBTree *tree,char *origin,char *destiny);

int main(void) {

    //C:\Users\Marcos\ClionProjects\SOPractica1Lloro\file.csv
    //C:\Users\Marcos\Desktop\Practica1SO2\src\file.csv
    RBTree *tree = NULL;
    char *filename;
    char *input;
    FILE *f = NULL;
  
    printOnGnuPlot();
    
    while(1){
        input = calloc(2,sizeof(char));
	    filename = calloc(100,sizeof(char));
        printf("__________________________\n");
        printf("Opciones:\n");
        printf("1 - Crear arbol\n");
        printf("2 - Almacenar arbol\n");
        printf("3 - Leer arbol\n");
        printf("4 - Grafica de retraso\n");
        printf("5 - Salir\n");
        printf("__________________________\n\n");
        scanf(" %1s",input);

        switch(input[0]){
            case '1': {
                printf("Introduca el nombre del archivo: ");
                scanf("%s",filename);
                if (tree != NULL) {
                    deleteTree(tree);
                }
                tree = createTree(filename);
                }
                break;
            case '2':
                printf("Introduca el nombre del archivo donde almacenarlo:\n");
                scanf("%s",filename);
                if(tree != NULL){
                    f=fopen(filename,"w");
                    writeNode(tree->root,f);
                    fclose(f);
                }
                break;
            case '3':
                printf("Introduca el nombre del archivo de donde leerlo:\n");
                scanf("%s",filename);
                tree = readTree(filename);
                break;
            case '4':
		if(tree!=NULL){
			char *origen;
			char *destino;
			origen = malloc(4*sizeof(char));
			destino = malloc(4*sizeof(char));
			printf("Introduca el nombre del aeropuerto origen:\n");
			scanf("%s",filename);
			printf("Introduca el nombre del aeropuerto destino:\n");
			scanf("%s",filename);
			writeGnuPlotData(tree,origen,destino);
			printOnGnuPlot();
		}else{
		  printf("El arbol esta vacio, imposible realizar la búsqueda\n");
		}
		break;
            case '5':
                if(tree != NULL){
                    deleteTree(tree);
                }
				free(input);
				free(filename);
                return 0;
            default:
                printf("ATENCION: Opcion no reconocida\n");
                printf("Introduca otra opcion\n");
                break;
        }
        free(input);
		free(filename);
    }
}

void writeGnuPlotData(RBTree *tree,char *origin,char *destiny){
  FILE *f;
  RBData *rbdata;
  ListData *data;
  int i;
  f = fopen("gnuplotData.data","w");
  if(f==NULL){
    printf("Error abriendo archivo para escribir gnuplotData.data\n");
    exit(0);
  }
  rbdata = findNode(tree,origin);
  data = findList(rbdata->destiny,destiny);
  for(i=0;i<7;i++){
    fprintf(f,"%d %d\n",data->delay[i],data->delay[i+7]);
  }
  fclose(f);
}

void printOnGnuPlot(){
  
  FILE *gp;
  gp = popen(GNUPLOT,"w");
  if(gp == NULL){
    printf("ERROR OPENING GNUPLOT\n");
    exit(0);
  }
  fprintf(gp,"plot 'gnuplotData.data' with lines\n");
  pclose(gp);
}

RBTree * createTree(char *filename){
    char **lines;
    FILE *fp;
    List **hash;
    RBTree *tree;
    tree = (RBTree *) malloc(sizeof(RBTree));

    initTree(tree);

    fp = fopen(filename, "r");
    if (!fp) {
        printf("Could not open file '%s'\n", filename);
        exit(1);
    }
    //while we dont reach the end of file
    /**
    while (fgets(line, MAXCHAR, fp) != NULL) {
        processLine(line, &tree);
        count(0);
    }
     **/

    lines = readNLines(fp,N);
    hash = generateHash(lines,MAXHASHSIZE,N);
    addHashToTree(hash,MAXHASHSIZE,tree);

    fclose(fp);
    //we free all auxiliary data used.
    freeLines(lines,N);
    deleteHash(hash,MAXHASHSIZE);

    return tree;
}

//This method adds a list to the hash, sets the two keys , delays and day of week.
void initHashList(List *list,char *origin, char *destiny, int dayOfWeek, int delay){

    ListData *listData;
    listData = findListBySelectingKey(list, destiny,0);

    if (listData != NULL) {

        /* We increment the number of times current item has appeared */
        listData->delay[dayOfWeek - 1] += delay;
        listData->delay[dayOfWeek - 1 + 7]++;
    } else {

        /* If the key is not in the list, allocate memory for the data and
        * insert it in the list */

        char *b;
        b = (char *) malloc(sizeof(char) * 4);
        b[0] = destiny[0];
        b[1] = destiny[1];
        b[2] = destiny[2];
        b[3] = '\0';
        char *c;
        c = (char *) malloc(sizeof(char) * 4);
        c[0] = origin[0];
        c[1] = origin[1];
        c[2] = origin[2];
        c[3] = '\0';
        listData = calloc(1,sizeof(ListData));
        listData->key = b;
        listData->key_sec = c;
        listData->delay[dayOfWeek - 1] = delay;
        listData->delay[dayOfWeek - 1 + 7] = 1;

        insertList(list, listData);
    }
}

//Function that returns an int value over from an string a seed and a hashSize.
int hashIndex(char *str,int seed,int hashSize){

    int sum,i,len;

    len = strlen(str);
    sum = 0;
    for(i = 0; i < len; i++)
        sum = sum * seed + (int)str[i];

    return sum % hashSize;

}

//Function that returns a vector of List that is our hash vector, str is the vector of strings
//readed from the file with other function. nLines is the number of lines.
List ** generateHash(char **str,int maxHashSize,int nLines){

    List ** hash;

    int j,k;

    char *origin;
    char *destiny;

    hash = (List **) calloc(maxHashSize,sizeof(List *));
    for(k=0;k<maxHashSize;k++){
        hash[k] = (List *) -1;
    }

    origin = (char *) calloc(4,sizeof(char));
    origin[3] = '\0';

    destiny = (char *) calloc(4,sizeof(char));
    destiny[3] = '\0';

    for(j=0;j<nLines;j++){

        int i = 0;
        //var used to count the number of commas
        int commaCounter = 0;
        /*letfFlag & rightFlag are used to flag both of
         *imits of the value we  need so in this case
         *they always are commas pointing the end of value
         */
        int leftFlag = 0;
        int rightFlag = 0;

        int hashValue = 0;
        //var used to store the day of week
        int dayOfWeek = 0;
        // var used to store the delay of flights
        int delay = 0;
        //This while will iterate over the lines of the file we are reading
        while (str[j][i] != '\0') {
            //When we find a comma
	    if (str[j][i] == ',') {
                //we store the left flag and right flag is where the 'new' comma is
                leftFlag = rightFlag;
                rightFlag = i;
                //we increment commacounter
                commaCounter++;
                switch (commaCounter) {
                    case 4:
                        str[j][rightFlag] = '\0';
                        dayOfWeek = atoi(&str[j][leftFlag + 1]);
                        str[j][rightFlag] = ',';
                        break;
                    case 15:
                        str[j][rightFlag] = '\0';
                        delay = atoi(&str[j][leftFlag + 1]);
                        str[j][rightFlag] = ',';
                        break;
                    case 17:
                        origin[0] = str[j][leftFlag + 1];
                        origin[1] = str[j][leftFlag + 2];
                        origin[2] = str[j][leftFlag + 3];
                        //printf("String: %s\n", origin);
                        break;
                    case 18: {
                        destiny[0] = str[j][leftFlag + 1];
                        destiny[1] = str[j][leftFlag + 2];
                        destiny[2] = str[j][leftFlag + 3];
                    }
                        break;
                    default:
                        break;
                }
            }
            i++;
        }

        hashValue = hashIndex(origin,2109,maxHashSize);
        if(hash[hashValue] == (List *) -1){
            hash[hashValue] = (List *) calloc(1,sizeof(List));
            initList(hash[hashValue]);
        }
        initHashList(hash[hashValue],origin,destiny,dayOfWeek,delay);

    }

    free(origin);
    free(destiny);

    return hash;

}

//Function that adds the hash map to the current tree.
void addHashToTree(List **hash,int maxHashSize, RBTree *tree){

    int i;

    for(i=0;i<maxHashSize;i++){
        if(hash[i]!=(List *)-1){
            RBData *treeData = NULL;
            treeData = findNode(tree, hash[i]->first->data->key_sec);

            if (treeData != NULL) {
                mergeLists(treeData,hash[i]);
            } else {

                /* If the key is not in the tree, allocate memory for the data
                 * and insert in the tree */
                char *b, *origin;
                origin = hash[i]->first->data->key_sec;
                b = (char *) calloc(4,sizeof(char));
                b[0] = origin[0];
                b[1] = origin[1];
                b[2] = origin[2];
                b[3] = '\0';
                treeData = malloc(sizeof(RBData));
                treeData->key = b;
                treeData->num = 1;
                treeData->destiny = copyList(hash[i]);
                insertNode(tree, treeData);
            }
        }
    }

}

//Function that adds a list to another list in the tree.
void mergeLists(RBData *treeData, List* hashList){

    int i,j,k;
    ListItem *currentItem,*currentTreeItem;
    currentItem = hashList->first;
    currentTreeItem = treeData->destiny->first;
    for(i=1;i<hashList->numItems;i++){
        for(j=1;j<treeData->destiny->numItems;j++){
            if(strcmp(currentTreeItem->data->key,currentItem->data->key)==0){
                for(k=0;k<14;k++){
                    currentTreeItem->data->delay[k]+=currentItem->data->delay[k];
                }
            }else{
                //ListData *auxiliar;
                //auxiliar = (ListData *) malloc(sizeof(ListData));
                //memcpy((void *)auxiliar,currentItem->data,sizeof(ListData));
                insertList(treeData->destiny, copyListData(currentItem->data));
            }
        }
    }
    treeData->num = treeData->destiny->numItems;
}

//Function that reads numberOfLines lines of the file.
char **readNLines(FILE * fp,int numberOfLines){

    int counter=0;
    char *line;
    char **lineVector;
    lineVector = (char **) calloc(numberOfLines,sizeof(char *));
    line = (char *) calloc(MAXCHAR,sizeof(char));

    while(fgets(line, MAXCHAR, fp) != NULL && counter<numberOfLines){

        lineVector[counter] = line;
        line = (char *) calloc(MAXCHAR,sizeof(char));

        counter++;
    }
    free(line);

    return lineVector;
}

//Function that copy a ListData struct to another.
ListData * copyListData(ListData *list){

    ListData *newListData;
    newListData = malloc(sizeof(ListData));

    memcpy(newListData->delay,list->delay,sizeof(int)*14);
    newListData->key = malloc(sizeof(char)*4);
    newListData->key_sec = malloc(sizeof(char)*4);
    strcpy(newListData->key,list->key);
    strcpy(newListData->key_sec,list->key_sec);

    return newListData;
}

//Function that copy a List struct to another.
List * copyList(List *list){

    int i;
    List *newList;
    ListItem *current;

    current = list->first;
    newList = malloc(sizeof(List));
    initList(newList);

    for(i=0;i<list->numItems;i++){
        insertList(newList,copyListData(current->data));
        current = current->next;
    }

    return newList;

}

//Function that deletes all the content of the hash and free's the hash.
void deleteHash(List **hash,int maxHashSize){

    int i;

    for(i=0;i<maxHashSize;i++){

        if(hash[i]!=(List *)-1){
            deleteList(hash[i]);
            free(hash[i]);
        }

    }

    free(hash);
}

//Function that free's all the readed lines.
void freeLines(char **strVec,int lineNumber){
	
	int i;
	for(i=0;i<lineNumber;i++){
		free(strVec[i]);
	}

	free(strVec);

}



List * readList(FILE *fp){

    int numberOfItems,i;
    char *keyForRead;
    List *list;
    ListData *data;
    list = (List *)malloc(sizeof(List));
    initList(list);


    fread(&numberOfItems,sizeof(int),1,fp);
    for(i=0;i<numberOfItems;i++){
        data = malloc(sizeof(ListData));
        
        keyForRead = malloc(4*sizeof(char));
        fread(keyForRead,sizeof(char),4,fp);
        data->key = keyForRead;

        keyForRead = malloc(4*sizeof(char));
        fread(keyForRead,sizeof(char),4,fp);
        data->key_sec = keyForRead;

        fread(data->delay,sizeof(int),14,fp);
        insertList(list,data);
    }
    return list;
}


RBTree * readTree(char * filename){
    FILE *f;
    RBTree * tree;
    RBData *data;
    char *key;
    tree = malloc(sizeof(tree));
    key = malloc(4*sizeof(char));
    initTree(tree);
    f = fopen(filename,"r");
    while(fread(key,sizeof(char),4,f)!=0){
        printf("%s\n",key);
        data = malloc(sizeof(RBData));
        data->key = key;
        data->destiny = readList(f);
        insertNode(tree,data);
        key = malloc(4*sizeof(char));
    }
    free(key);
    fclose(f);
    return tree;
}

