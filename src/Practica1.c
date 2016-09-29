#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

#include "red-black-tree.h"

#define MAXCHAR  100

void readCSV(char *filename)
{
  RBTree *tree;
  RBData *treeData;
  ListData *listData;
  List *list;
  
  FILE *fp;
  char *line;
  char * a;
	    
  int lineCounter=0;
  
  a = (char *) malloc(sizeof(char)*4);
  line = (char *) malloc(sizeof(char) * MAXCHAR);
  tree = (RBTree *) malloc(sizeof(RBTree));

  
  initTree(tree);

  fp = fopen(filename, "r");
  if (!fp) {
    printf("Could not open file '%s'\n", filename);
    exit(1);
  }

  while (fgets(line, MAXCHAR, fp) != NULL){
    
    int i=0;
    int commaCounter=0;
    int leftFlag=0;
    int rightFlag=0;
    while (line[i] != '\0') {
      int dayOfWeek = 0;
      int delay = 0;
      if(line[i]==','){
	leftFlag=rightFlag;
	rightFlag=i;
	commaCounter++;
      
      
	switch(commaCounter){
	  case 4:
	    line[rightFlag] = '\0';
	    dayOfWeek = atoi(&line[leftFlag+1]);
	    line[rightFlag] = ',';
	    break;
	  case 15:
	    line[rightFlag] = '\0';
	    delay = atoi(&line[leftFlag+1]);
	    line[rightFlag] = ',';
	    break;
	  case 17:
	    a[0] = line[leftFlag+1];
	    a[1] = line[leftFlag+2];
	    a[2] = line[leftFlag+3];
	    a[3] = '\0';
	    printf("String: %s\n",a);
	    treeData = findNode(tree, a);
	    
	    if (treeData != NULL) {

	    /* If the key is in the tree increment 'num' */
	      treeData->num++;
	    } else {

      /* If the key is not in the tree, allocate memory for the data
       * and insert in the tree */
	      char *b;
	      b = (char *) malloc(sizeof(char)*4);
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
	  case 18:
	    
	    
	    list = treeData->destiny;
	    listData = findList(list, a); 

	    if (listData != NULL) {

	      /* We increment the number of times current item has appeared */
	      listData->delay[dayOfWeek-1]+=delay;
	      listData->delay[dayOfWeek-1+7]++;
	    } else {

	      /* If the key is not in the list, allocate memory for the data and
	      * insert it in the list */

	       char *b;
	      b = (char *) malloc(sizeof(char)*4);
	      b[0] = a[0];
	      b[1] = a[1];
	      b[2] = a[2];
	      b[3] = '\0';
	      listData = malloc(sizeof(ListData));
	      listData->key = b;
	      listData->delay[dayOfWeek-1]=delay;
	      listData->delay[dayOfWeek-1+7]=1;

	      insertList(list, listData);
	    }
	    
	    break;
	  default:
	    break;
	}
      }
      i++;
    }
    printf("Line: %d\n",lineCounter);
    lineCounter++;
  }
  
  deleteTree(tree);
  fclose(fp);
  free(a);
  free(line);
  
}


int dotcommaCounter(char *str){
  int npunticoma = 0;
  int i=0;
  
  while (str[i] != '\0') {
    if (str[i] == ';')
      npunticoma++;
    i++;
  }
  
  return npunticoma;
}

int main(void){
  
  
  readCSV("test.csv");
  
  
  return 0;
}