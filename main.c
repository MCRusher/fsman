#include <stdio.h>
#include <assert.h>

#include "fsman.h"

int main(void) {
  //assert(fsman_makeFile("./lamo.txt") == fserror_EXISTS);
  //assert(fsman_makeFolder("./xd/xd2/xd3/xd4/", true) == fserror_SUCCESS);
  printf("error: %d\n"fsman_delete("./xd", true));
  printf("Hello World\n");
  return 0;
}