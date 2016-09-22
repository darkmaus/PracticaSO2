/**
 *
 * Main file 
 * 
 * Lluis Garrido, 2016.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

#define MAXCHAR  100

/**
 *
 * This file reads and outputs the lines of a CSV file. 
 *
 */

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

/**
 *
 * Main function. 
 *
 */

int main(int argc, char **argv)
{
  if (argc != 2) {
    printf("Usage: %s <fitxer.txt>\n", argv[0]);
    exit(1);
  }

  readCSV(argv[1]);

  return 0;
}

