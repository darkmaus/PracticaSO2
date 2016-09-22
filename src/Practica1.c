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
  FILE *fp;
  char *line;
  char * a;
  int lineCounter=0;
  
  a = (char *) malloc(sizeof(char)*4);
  line = (char *) malloc(sizeof(char) * MAXCHAR);
  tree = (RBTree *) malloc(sizeof(RBTree));

  a[3] = '\0';
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
      if(line[i]==','){
	leftFlag=rightFlag;
	rightFlag=i;
	commaCounter++;
      
      
	switch(commaCounter){
	  case 4:
	    //line[rightFlag] = '\0';
	    //printf("String: %s\n",&line[leftFlag+1]);
	    //line[rightFlag] = ',';
	    break;
	  case 15:
	    break;
	  case 17:
	    a[0] = line[leftFlag+1];
	    a[1] = line[leftFlag+2];
	    a[2] = line[leftFlag+3];
	    printf("String: %s\n",a);
	    treeData = findNode(tree, a);
	    
	    if (treeData != NULL) {

	    /* If the key is in the tree increment 'num' */
	      treeData->num++;
	    } else {

      /* If the key is not in the tree, allocate memory for the data
       * and insert in the tree */

	      treeData = malloc(sizeof(RBData));
	      treeData->key = a;
	      treeData->num = 1;

	      insertNode(tree, treeData);
	    }
	    break;
	  case 18:
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
  
  
  readCSV("file.csv");
  
  
  return 0;
}