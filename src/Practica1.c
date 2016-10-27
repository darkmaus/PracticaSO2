#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

#include "red-black-tree.h"

#define MAXCHAR  200
#define MAXHASHSIZE 500

void destinyFunc(List *list, char *destiny, int dayOfWeek, int delay);
void originFunc(RBTree *tree,RBData ** treeData,char *origin);
void processLine(char *line, RBTree **tree);
void count(int reset);
int hashIndex(char *str,int seed,int hashSize);
void initHashList(List *list,char *origin, char *destiny, int dayOfWeek, int delay);
char **readNLines(FILE * fp,int numberOfLines);
List ** generateHash(char **str,int maxHashSize,int nLines);
void addHashToTree(List **hash,int maxHashSize, RBTree *tree);
void mergeLists(RBData *treeData, List* hashList);
void deleteHash(List **hash,int maxHashSize);
ListData * copyListData(ListData *list);
List * copyList(List *list);

void readCSV(char *filename) {
    RBTree *tree;

    FILE *fp;
    char *line;

    line = (char *) malloc(sizeof(char) * MAXCHAR);
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
    addHashToTree(generateHash(readNLines(fp,100),MAXHASHSIZE,100),MAXHASHSIZE,tree);
    // we delete the tree from the memory
    deleteTree(tree);
    //we close the file
    fclose(fp);
    //we free all auxiliary data used.

    free(line);

}

int main(void) {

    //C:\Users\Marcos\ClionProjects\SOPractica1Lloro\file.csv
    readCSV("C:\\Users\\Marcos\\ClionProjects\\SOPractica1Lloro\\file.csv");


    return 0;
}

void destinyFunc(List *list, char *destiny, int dayOfWeek, int delay){

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
        listData = calloc(1,sizeof(ListData));
        listData->key = b;
        listData->delay[dayOfWeek - 1] = delay;
        listData->delay[dayOfWeek - 1 + 7] = 1;

        insertList(list, listData);
    }
}

void originFunc(RBTree *tree,RBData ** treeData,char *origin){

    (*treeData) = findNode(tree, origin);

    if ((*treeData) != NULL) {
        /* If the key is in the tree increment 'num' */
        (*treeData)->num++;
    } else {

        /* If the key is not in the tree, allocate memory for the data
         * and insert in the tree */
        char *b;
        b = (char *) calloc(4,sizeof(char));
        b[0] = origin[0];
        b[1] = origin[1];
        b[2] = origin[2];
        b[3] = '\0';
        (*treeData) = malloc(sizeof(RBData));
        (*treeData)->key = b;
        (*treeData)->num = 1;
        (*treeData)->destiny = calloc(1,sizeof(List));
        initList((*treeData)->destiny);
        insertNode(tree, (*treeData));
    }
}

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

void processLine(char *line, RBTree **tree){

    char *origin;
    char *destiny;

    origin = (char *) calloc(4,sizeof(char));
    origin[3] = '\0';

    destiny = (char *) calloc(4,sizeof(char));
    destiny[3] = '\0';


    RBData *treeData;
    //var used to iterate over the line chars
    int i = 0;
    //var used to count the number of commas
    int commaCounter = 0;
    /*letfFlag & rightFlag are used to flag both of
     *imits of the value we  need so in this case
     *they always are commas pointing the end of value
     */
    int leftFlag = 0;
    int rightFlag = 0;
    //var used to store the day of week
    int dayOfWeek = 0;
    // var used to store the delay of flights
    int delay = 0;
    //This while will iterate over the lines of the file we are reading
    while (line[i] != '\0') {
        //When we find a comma
        if (line[i] == ',') {
            //we store the left flag and right flag is where the 'new' comma is
            leftFlag = rightFlag;
            rightFlag = i;
            //we increment commacounter
            commaCounter++;
            switch (commaCounter) {
                case 4:
                    line[rightFlag] = '\0';
                    dayOfWeek = atoi(&line[leftFlag + 1]);
                    line[rightFlag] = ',';
                    break;
                case 15:
                    line[rightFlag] = '\0';
                    delay = atoi(&line[leftFlag + 1]);
                    line[rightFlag] = ',';
                    break;
                case 17:
                    origin[0] = line[leftFlag + 1];
                    origin[1] = line[leftFlag + 2];
                    origin[2] = line[leftFlag + 3];
                    //printf("String: %s\n", origin);
                    break;
                case 18: {
                    destiny[0] = line[leftFlag + 1];
                    destiny[1] = line[leftFlag + 2];
                    destiny[2] = line[leftFlag + 3];
                }
                    break;
                default:
                    break;
            }
        }
        i++;
    }

    if(strcmp(origin,"")!=0 && strcmp(destiny,"")!=0){
        originFunc(*tree, &treeData, origin);
        destinyFunc(treeData->destiny, destiny, dayOfWeek, delay);
    }

    free(origin);
    free(destiny);
}

void count(int reset){

    static int counter = 0;
    if(reset==0){
        counter++;
    }else{
        counter=0;
    }
    printf("%d\n",counter);

}

int hashIndex(char *str,int seed,int hashSize){

    int sum,i,len;

    len = strlen(str);
    sum = 0;
    for(i = 0; i < len; i++)
        sum = sum * seed + (int)str[i];

    return sum % hashSize;

}

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

    deleteHash(hash,maxHashSize);
}

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