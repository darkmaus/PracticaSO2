#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <pthread.h>

#include "red-black-tree.h"

#define MAXCHAR  200
#define MAXHASHSIZE 500
#define N 1000
#define NUMBERTHREADS 8
#define SIZEOFBUFFER 8

//Lista para los threads
pthread_t ntid[NUMBERTHREADS+1];
//Mutex para los productores y los consumidores
pthread_mutex_t mutexR = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexW = PTHREAD_MUTEX_INITIALIZER;

//Define para facilitar el gnuplot
#define GNUPLOT "gnuplot -persist"

//Variables para controlar el lugar actual de lectura y el lugar actual de escritura
//para que los threads no se pisen a la hora de escribir en el buffer, y de leer en el
//buffer
//Contador indica la cantidad de posiciones del buffer que hay utilizadas actualmente
int w=0,r=0,contador=0;
//Inicializamos los threads
pthread_cond_t condP = PTHREAD_COND_INITIALIZER;
pthread_cond_t condC = PTHREAD_COND_INITIALIZER;
//Variable que indica si hemos terminado de leer el archivo
int finalDeLectura=0;

//Estructura que produce el productor y que es pasada a los consumidores, contiene las lineas leidas
//y la cantidad de lineas que ha leido
typedef struct LineData_{
  char **vectorLines;
  int numberOfLines;
}LineData;

int hashIndex(char *str,int seed,int hashSize);
void initHashList(List *list,char *origin, char *destiny, int dayOfWeek, int delay);
LineData *readNLines(FILE * fp,int numberOfLines);
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
void printRBData(RBData *data);

//Parametros que le pasamos a los threads, el arbol a inicializar, el archivo a leer, y el buffer donde
//escribimos lo producido y le pasamos a los consumidores
typedef struct ThreadParameters_{
  RBTree * tree;
  FILE *file;
  LineData **buffer;
}ThreadParameters;

void *coreFunction(void * args);
void *coreProductorFunction(void * args);
void *coreConsumerFunction(void * args);

int main(void) {

  RBTree *tree = NULL;
  char *filename;
  char *input;
  FILE *f = NULL;
  
  //Bucle que repite las opciones hasta que se selecciona la 5, que hace que termine el programa
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
    printf("6 - Imprimir datos por pantalla\n");
    printf("__________________________\n\n");
    scanf(" %1s",input);

    switch(input[0]){
      case '1':
	printf("Introduca el nombre del archivo: ");
	scanf("%s",filename);
	if (tree != NULL) {
	    deleteTree(tree);
	}
	tree = createTree(filename);
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
	if(tree!=NULL){
	  deleteTree(tree);
	}
	tree = readTree(filename);
	break;
      case '4':
	if(tree!=NULL){
	  char *origen;
	  char *destino;
	  origen = malloc(4*sizeof(char));
	  destino = malloc(4*sizeof(char));
	  printf("Introduca el nombre del aeropuerto origen:\n");
	  scanf("%s",origen);
	  printf("Introduca el nombre del aeropuerto destino:\n");
	  scanf("%s",destino);
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
      case '6':
	if(tree!=NULL){
	  printRBData(tree->root->data);
	}
	break;
      default:
	printf("ATENCION: Opcion no reconocida\n");
	printf("Introduca otra opcion\n");
	break;
    }
    free(input);
    free(filename);
  }
}

//Método que imprime por salida estandar los datos que contiene el arbol, si es que contiene datos
void printRBData(RBData *data){
  int i,j;
  ListItem *current;
  printf("Origin airport: %s\n",data->key);
  current = data->destiny->first;
  for(i=0;i<(data->destiny->numItems);i++){
    printf("\tDestiny: %s\n",current->data->key);
    for(j=0;j<7;j++){
      printf("\t\tDay of week %d ,%d trips,accumulated delay %d.\n",(j+1),current->data->delay[j+7],current->data->delay[j]);
    }
    current = current->next;
  }
}

//Método que escribe en el gnuplot y muestra una grafica en funcion del dia de la semana el retraso,
//este método solo escribe un archivo que lo lee printOnGnuplot
void writeGnuPlotData(RBTree *tree,char *origin,char *destiny){
  FILE *f;
  RBData *rbdata;
  ListData *data;
  int i,finaldelay[14];
  
  for(i=0;i<14;i++){
    finaldelay[i] = 0;
  }
  
  f = fopen("gnuplotData.data","w");
  if(f==NULL){
    printf("Error abriendo archivo para escribir gnuplotData.data\n");
    exit(0);
  }
  rbdata = findNode(tree,origin);
  if(rbdata!=NULL){
    data = findList(rbdata->destiny,destiny);
    if(data!=NULL){
      for(i=0;i<14;i++){
	finaldelay[i] = data->delay[i];
      }
    }
  }
  rbdata = findNode(tree,destiny);
  if(rbdata!=NULL){
    data = findList(rbdata->destiny,origin);
    if(data!=NULL){
      for(i=0;i<14;i++){
	finaldelay[i] += data->delay[i];
      }
    }
  }
  for(i=0;i<7;i++){
    if(finaldelay[i+7]!=0){
      fprintf(f,"%d %f\n",i,((float)finaldelay[i])/finaldelay[i+7]);
    }else{
      fprintf(f,"%d %f\n",i,0.0);
    }
  }
  fclose(f);
}

//Muestra la grafica en una nueva ventana
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

//Método principal de creación de hilos
RBTree * createTree(char *filename){
    
    FILE *fp;
    RBTree *tree;
    ThreadParameters * parameters;
    int i;
    parameters = malloc(sizeof(ThreadParameters));
    tree = (RBTree *) malloc(sizeof(RBTree));
    initTree(tree);

    fp = fopen(filename, "r");
    if (!fp) {
        printf("Could not open file '%s'\n", filename);
        exit(1);
    }
    //Inicializo los parametros una vez he creado el espacio para ellos
    parameters->file = fp;
    parameters->tree = tree;
    parameters->buffer = (LineData **) malloc(SIZEOFBUFFER*sizeof(LineData *));
    //Creo n threads para el consumo de lo que produzca el productor
    for(i=0;i<NUMBERTHREADS;i++){
      pthread_create(&ntid[i],NULL,coreConsumerFunction,(void *)parameters);
    }
    //Creo al productor
    pthread_create(&ntid[NUMBERTHREADS],NULL,coreProductorFunction,(void *)parameters);
    
    //Espero a que terminen todos
    for(i=0;i<NUMBERTHREADS;i++){
      pthread_join(ntid[i],(void *)parameters);
    }
    pthread_join(ntid[NUMBERTHREADS],(void *)parameters);
    
    //Libero las variables para las que he reservado memoria
    free(parameters->buffer);
    free(parameters);
    fclose(fp);
    return tree;
}

//Funcion principal para un thread, ésta consiste en utilizar dos mutex, uno para bloquear
//la lectura y el otro para bloquear la escritura, el resto sique siendo lo mismo que la practica anterior,
void *coreFunction(void * args){
  
  FILE *fp;
  RBTree * tree;
  LineData *data;
  List **hash;
  ThreadParameters *parameters;
  parameters = (ThreadParameters *)args;
  
  fp = parameters->file;
  tree = parameters->tree;
  //printf("FILE POINTER%d\n",fp);
  while(1){
    pthread_mutex_lock(&mutexR);
    data = readNLines(fp,N);
    pthread_mutex_unlock(&mutexR);
    //En el caso en que no haya leido lineas, libero la memoria reservada para data, y retorno.
    if(data->numberOfLines==0){
      free(data);
      return ((void *)args);
    }
    hash = generateHash(data->vectorLines,MAXHASHSIZE,data->numberOfLines);
    
    pthread_mutex_lock(&mutexW);
    addHashToTree(hash,MAXHASHSIZE,tree);
    pthread_mutex_unlock(&mutexW);
    freeLines(data->vectorLines,data->numberOfLines);
    free(data);
    deleteHash(hash,MAXHASHSIZE);
  }
  return ((void *)args);
  
}

//Método principal del productor
void *coreProductorFunction(void * args){
  
  FILE *fp;
  ThreadParameters *parameters;
  parameters = (ThreadParameters *)args;
  LineData *data;
  LineData **buffer;
  int finalOfProduction;
  int i;
  //Guardo en variables locales los atributos por comodidad.
  fp = parameters->file;
  buffer = parameters->buffer;
  //Variable para indicar que he terminado de producir
  finalOfProduction = 0;
  
  while(!finalOfProduction){
  	//Leo las lineas
    data = readNLines(fp,N);
 	//"Cojo" el mutexW
    pthread_mutex_lock(&mutexW);
    //En el caso en que no tenga mas espacio en el buffer espero a que consuman espacio, por ello
    //tengo el wait para esperar a que me despierten cuando consuman algo de lo que ha producido
    while(contador==SIZEOFBUFFER){
       pthread_cond_wait(&condP,&mutexW);
    }
    //En el caso en que no haya leido lineas, cambio la variable y libero espacio reservado
    if(data->numberOfLines==0){
      finalOfProduction = 1;
      for(i=0;i<N;i++){
		free(data->vectorLines[i]);
      }
      free(data->vectorLines);
      free(data);
      //En caso que no, copio la referencia en el buffer e incremento la posicion en la que escribire
    }else{
    buffer[w] = data;
    w=(w+1)%SIZEOFBUFFER;
    //Incremento el contador para indicar que hay una posicion mas ocupada en el buffer
    contador++;
    }
    //Despierto a un consumido en el caso en que haya alguno durmiendo en condC
    pthread_cond_signal(&condC);
    //Desbloqueo la llave
    pthread_mutex_unlock(&mutexW);
    //Si he terminado, aviso a todos los productores para que terminen y cambio la variable global para
    //indicar que terminado de leer
    if (finalOfProduction == 1){
      finalDeLectura=1;
      pthread_mutex_lock(&mutexW);
      pthread_cond_broadcast(&condC);
      pthread_mutex_unlock(&mutexW);
    }
  }
  return ((void *)args);
}

//El consumer es parecido al productor
void *coreConsumerFunction(void * args){
  ThreadParameters *parameters;
  parameters = (ThreadParameters *)args;
  LineData *data;
  List **hash;
  LineData **buffer;
  RBTree * tree;
  int i;
  
  //Por comodidad, asigno a variables locales lo pasado por argumento
  tree = parameters->tree;
  buffer = parameters->buffer;
  
  while(1){
  	//Cojo la llave mutexW
    pthread_mutex_lock(&mutexW);
    //Si no hay nada en el buffer y no he terminado de leer el hilo se pone a dormir hasta que 
    //me avisen, si he terminado, aviso al resto de productores y libero la llave
    while(contador==0){
      if(finalDeLectura){
	pthread_cond_broadcast(&condC);
	pthread_mutex_unlock(&mutexW);
	return ((void *)args);
      }
      pthread_cond_wait(&condC,&mutexW);
    }
    //Copio la direccion de los datos a consumir en la variable data e incremento la variable de lectura,
    //y decremento el contador para indicar que hay un hueco mas libre en el buffer
    data = buffer[r];
    r=(r+1)%SIZEOFBUFFER;
    contador--;
    //Aviso a los productores de que he consumido un dato
    pthread_cond_signal(&condP);
    //Libero la llava
    pthread_mutex_unlock(&mutexW);
    
    //Consumo los datos
    hash = generateHash(data->vectorLines,MAXHASHSIZE,data->numberOfLines);
    
    //Cojo la llave para añadir los datos al arbol y luego libero la llave una vez los he añadido
    pthread_mutex_lock(&mutexR);
    addHashToTree(hash,MAXHASHSIZE,tree);
    pthread_mutex_unlock(&mutexR);
    
    //Libero la memoria de las variables que ya he utilizado y no voy a volver a utilizar
    for(i=0;i<N;i++){
      free(data->vectorLines[i]);
    }
    free(data->vectorLines);
    free(data);
    deleteHash(hash,MAXHASHSIZE);
  }
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

    int i,j,k,found;
    ListItem *currentItem,*currentTreeItem;
    currentItem = hashList->first;
    
    
    for(i=1;i<hashList->numItems;i++){
	currentTreeItem = treeData->destiny->first;
        found=0;
        for(j=1;j<treeData->destiny->numItems;j++){
            if(strcmp(currentTreeItem->data->key,currentItem->data->key)==0){
	        found=1;
                for(k=0;k<14;k++){
                    currentTreeItem->data->delay[k]+=currentItem->data->delay[k];
                }
	    }
	    currentTreeItem = currentTreeItem->next;
	}
	if (!found) {
                //ListData *auxiliar;
                //auxiliar = (ListData *) malloc(sizeof(ListData));
                //memcpy((void *)auxiliar,currentItem->data,sizeof(ListData));
                insertList(treeData->destiny, copyListData(currentItem->data));
        }
        currentItem = currentItem->next;
    }
    treeData->num = treeData->destiny->numItems;
}

//Function that reads numberOfLines lines of the file.
LineData *readNLines(FILE * fp,int numberOfLines){

    int counter=0;
    char *line;
    char *returnPointer;
    char **lineVector;
    LineData *data;
    
    data = (LineData *) calloc(1,sizeof(LineData));
    lineVector = (char **) calloc(numberOfLines,sizeof(char *));
    line = (char *) calloc(MAXCHAR,sizeof(char));
    returnPointer = fgets(line, MAXCHAR, fp);
    
    while( returnPointer != NULL && counter<numberOfLines){

        lineVector[counter] = line;
        line = (char *) calloc(MAXCHAR,sizeof(char));
        counter++;
	returnPointer = fgets(line, MAXCHAR, fp);
    }
    
    if(returnPointer==NULL){
      data->numberOfLines = counter;
    }else{
      data->numberOfLines = numberOfLines;
    }
    data->vectorLines = lineVector;
    free(line);
    return data;
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

