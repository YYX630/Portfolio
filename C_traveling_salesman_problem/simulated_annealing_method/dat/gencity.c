// generate a binary data for cities (TSP)
// the first int means the number of cities
// the following values are x_0, y_0, 
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strerror()
#include <errno.h> // errno, ERANGE
#include <assert.h> // assert()

int load_int(const char *argvalue)
{
  long nl;
  char *e;
  errno = 0; // errno.h で定義されているグローバル変数を一旦初期化
  nl = strtol(argvalue,&e,10);
  if (errno == ERANGE){
    fprintf(stderr,"%s: %s\n",argvalue,strerror(errno));
    exit(1);
  }
  if (*e != '\0'){
    fprintf(stderr,"%s: an irregular character '%c' is detected.\n",argvalue,*e);
    exit(1);
  }
  return (int)nl;
}


int main(int argc, char **argv)
{
  const int width = 70;
  const int height = 40;
  const int max_cities = 100;

  if(argc != 4){
    fprintf(stderr, "usage: %s <number of cities> <random seed> <outputfilename>\n",argv[0]);
    return EXIT_FAILURE;
  }
  int nc = load_int(argv[1]);
  assert( nc > 1 && nc <= max_cities);
  int seed = load_int(argv[2]);
  srand(seed);

  int *data = (int*)malloc(sizeof(int)*2*nc);
  for (int i = 0 ; i < nc ; i++){
    data[2*i] = rand() % (width - 10) + 5;;
    data[2*i+1] = rand() % (height - 10) + 5;;
  }

  FILE *fp;
  if ((fp = fopen(argv[3],"wb")) == NULL){
    fprintf(stderr, "%s: cannot open file.\n",argv[3]);
    return EXIT_FAILURE;
  }
  fwrite(&nc,sizeof(int),1,fp);
  fwrite(data,sizeof(int),2*nc,fp);
  fclose(fp);
  
  return EXIT_SUCCESS;
}
