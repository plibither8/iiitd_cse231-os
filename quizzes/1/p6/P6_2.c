#include <stdio.h>
#include <stdlib.h>

int main()
{
  char myName[] = "Mihir";

  FILE *fptr;
  fptr = fopen("./DISK_SAVE.txt", "w");

  // Error handling
  if (fptr == NULL)
  {
    printf("ERROR!");
    exit(1);
  }

  fprintf(fptr, "%s", myName);
  fclose(fptr);

  return 0;
}
