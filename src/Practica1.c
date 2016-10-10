#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

#include "red-black-tree.h"

#define MAXCHAR  100




void readCSV(char *filename) {
    RBTree *tree;
    RBData *treeData;
    ListData *listData;
    List *list;

    FILE *fp;
    char *line;
    char *a;
    int lineCounter = 0;

    a = (char *) malloc(sizeof(char) * 4);
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
                        a[0] = line[leftFlag + 1];
                        a[1] = line[leftFlag + 2];
                        a[2] = line[leftFlag + 3];
                        a[3] = '\0';
                        printf("String: %s\n", a);
                        treeData = findNode(tree, a);

                        if (treeData != NULL) {
                            /* If the key is in the tree increment 'num' */
                            treeData->num++;
                        } else {

                            /* If the key is not in the tree, allocate memory for the data
                             * and insert in the tree */
                            char *b;
                            b = (char *) malloc(sizeof(char) * 4);
                            b[0] = a[0];
                            b[1] = a[1];
                            b[2] = a[2];
                            b[3] = '\0';
                            treeData = malloc(sizeof(RBData));
                            treeData->key = b;
                            treeData->num = 1;
                            treeData->destiny = malloc(sizeof(List));
                            initList(treeData->destiny);
                            insertNode(tree, treeData);
                        }
                        break;
                    case 18: {
                        char *c;
                        c=malloc(sizeof(char)*4);
                        c[0] = line[leftFlag + 1];
                        c[1] = line[leftFlag + 2];
                        c[2] = line[leftFlag + 3];
                        c[3] = '\0';

                        list = treeData->destiny;
                        listData = findList(list, c);

                        if (listData != NULL) {

                            /* We increment the number of times current item has appeared */
                            listData->delay[dayOfWeek - 1] += delay;
                            listData->delay[dayOfWeek - 1 + 7]++;
                        } else {

                            /* If the key is not in the list, allocate memory for the data and
                            * insert it in the list */

                            char *b;
                            b = (char *) malloc(sizeof(char) * 4);
                            b[0] = c[0];
                            b[1] = c[1];
                            b[2] = c[2];
                            b[3] = '\0';
                            listData = calloc(1,sizeof(ListData));
                            listData->key = b;
                            listData->delay[dayOfWeek - 1] = delay;
                            listData->delay[dayOfWeek - 1 + 7] = 1;

                            insertList(list, listData);
                        }
                    }
                        break;
                    default:
                        break;
                }
            }
            i++;
        }
        printf("Line: %d\n", lineCounter);
        lineCounter++;
    }
    // we delete the tree from the memory
    deleteTree(tree);
    //we close the file
    fclose(fp);
    //we free all auxiliary data used.
    free(a);
    free(line);

}

int main(void) {


    readCSV("file.csv");


    return 0;
}