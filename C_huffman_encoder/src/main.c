#include <stdio.h>
#include <stdlib.h>
#include "encode.h"

int main(int argc, char **argv)
{
  if (argc != 2) {
    fprintf(stderr, "usage: %s <filename>\n",argv[0]);
    exit(1);
  }

  encode(argv[1]);

  return EXIT_SUCCESS;
}
