#include <stdio.h>
#include <string.h> 

#define MAXCHAR    4
#define HASH_SIZE  100

int main(void)
{
  unsigned int i, len, seed, sum, hash;
  char cadena[MAXCHAR];

  strcpy(cadena, "JFK");

  len = strlen(cadena); 
  sum = 0;
  seed = 131;
  for(i = 0; i < len; i++)
    sum = sum * seed + (int)cadena[i];

  hash = sum % HASH_SIZE;

  printf("El valor de hash de %s es %d\n", cadena, hash);
}
