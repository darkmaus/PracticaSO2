#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

#include "red-black-tree.h"

#define MAXCHAR  100

void destinyFunc(List *list, char *destiny, int dayOfWeek, int delay);
void originFunc(RBTree *tree,RBData ** treeData,char *origin);
void processLine(char *line, char *origin, char *destiny, RBTree **tree);
void count(int reset);

void readCSV(char *filename) {
    RBTree *tree;

    FILE *fp;
    char *line;

    char *origin;
    char *destiny;

    origin = (char *) malloc(sizeof(char) * 4);
    origin[3] = '\0';

    destiny = (char *) malloc(sizeof(char)*4);
    destiny[3] = '\0';

    line = (char *) malloc(sizeof(char) * MAXCHAR);
    tree = (RBTree *) malloc(sizeof(RBTree));

    initTree(tree);

    fp = fopen(filename, "r");
    if (!fp) {
        printf("Could not open file '%s'\n", filename);
        exit(1);
    }
    //while we dont reach the end of file
    while (fgets(line, MAXCHAR, fp) != NULL) {
        processLine(line, origin, destiny, &tree);
        count(0);
    }
    // we delete the tree from the memory
    deleteTree(tree);
    //we close the file
    fclose(fp);
    //we free all auxiliary data used.
    free(origin);
    free(destiny);
    free(line);

}

int main(void) {

    //C:\Users\Marcos\ClionProjects\SOPractica1Lloro\file.csv
    readCSV("C:\\Users\\Marcos\\ClionProjects\\SOPractica1Lloro\\file.csv");


    return 0;
}

void destinyFunc(List *list, char *destiny, int dayOfWeek, int delay){

    ListData *listData;
    listData = findList(list, destiny);

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
        b = (char *) malloc(sizeof(char) * 4);
        b[0] = origin[0];
        b[1] = origin[1];
        b[2] = origin[2];
        b[3] = '\0';
        (*treeData) = malloc(sizeof(RBData));
        (*treeData)->key = b;
        (*treeData)->num = 1;
        (*treeData)->destiny = malloc(sizeof(List));
        initList((*treeData)->destiny);
        insertNode(tree, (*treeData));
    }
}

void processLine(char *line, char *origin, char *destiny, RBTree **tree){

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
    originFunc(*tree,&treeData,origin);
    destinyFunc(treeData->destiny, destiny, dayOfWeek, delay);

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