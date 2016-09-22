#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

#define MAXCHAR  100

void readCSV(char *filename)
{
  FILE *fp;
  char *line;

  line = (char *) malloc(sizeof(char) * MAXCHAR);

  fp = fopen(filename, "r");
  if (!fp) {
    printf("Could not open file '%s'\n", filename);
    exit(1);
  }

  while (fgets(line, MAXCHAR, fp) != NULL)
     printf("%s", line);
  
  fclose(fp);

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