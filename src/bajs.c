#include "refmem.h"
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  for (int i = 0; i < 100 ; i++) {
    char *t = strdup2("testare");
    release(t);
  }
  shutdown();
  /* for (int i = 0; i < 100; i++) { */
  /*   char *t = strdup("testare"); */
  /*   free(t); */
  /* } */
  return 0;
}
