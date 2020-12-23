/* Name: Mihir Chaturvedi
   Roll_Number: 2019061 */

#include <sys/file.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINE_SIZE 1000

int main(int argc, char **argv) {
  // Argument count checks
  if (argc < 2) {
    fprintf(stderr, "No argument supplied - Please specify filename.\n");
    return 1;
  }

  printf(
    "Choose an operation:\n"
    "\t1. Edit the file\n"
    "\t2. Exit\n"
  );

  // While loop until valid choice
  char choice;
  do scanf("%c", &choice);
  while (choice != '1' && choice != '2');

  // Exit if choice is 2
  if (choice == '2') exit(0);

  // Get the filename
  char *filename = argv[1];

  // Open the file in append mode, get its pointer
  FILE *file_pointer = fopen(filename, "a+");
  int file_fd = fileno(file_pointer);

  // Attempt to acquire the lock on the entire file
  if (flock(file_fd, LOCK_EX | LOCK_NB) == -1) {
    if (errno == EWOULDBLOCK)
      // If the file is already locked
      fprintf(stderr, "FILE ALREADY LOCKED!\n");
    else
      fprintf(stderr, "An error occured while attempting to acquire lock\n");
  }
  else
    // We have successfully acquired locked
    fprintf(stdout, "File lock acquired successfully.\n");

  // Ready to print file contents
  fprintf(stdout, "File contents:\n==============");

  // Sleep for a second to ensure second program
  // can not acquire lock
  sleep(1);

  // Print file content line-by-line in loop
  char content_line_buffer[MAX_LINE_SIZE];
  while (fgets(content_line_buffer, MAX_LINE_SIZE, file_pointer))
    fputs(content_line_buffer, stdout);

  // ctrl+d to save and exit
  fprintf(stdout, "\nStart entering text, EOF to save and exit:\n");

  // get line by line and directly print to file
  char file_contents[MAX_LINE_SIZE];
  char new_line[MAX_LINE_SIZE];
  while (fgets(new_line, MAX_LINE_SIZE, stdin))
    fputs(new_line, file_pointer);

  // Close the file and exit
  fprintf(stdout, "Saved, exiting...\n");
  fclose(file_pointer);

  return 0;
}
