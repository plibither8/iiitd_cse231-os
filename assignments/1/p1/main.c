#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// Error message constants
char *ERR_FILE_OPEN = "ERR! File could not be opened properly.";
char *ERR_FILE_SEEK = "ERR! Could not seek in file.";
char *ERR_FILE_READ = "ERR! Coult not read file.";
char *ERR_FILE_CLOSE = "ERR! Could not close file.";
char *ERR_WRITE = "ERR! Could not write to specified stream";
char *ERR_FORK = "ERR! Could not fork parent.";

// Simple wrapper to take in error message
// and print out to stderr stream
void throw_error(char *msg)
{
  fprintf(stderr, "%s\n", msg);
  exit(1);
}

// Open the csv file and produce error
// if it couldn't be opened
int open_file()
{
  int fd = open("marks.csv", O_RDONLY);
  if (fd == -1)
    throw_error(ERR_FILE_OPEN);

  return fd;
}

// Get the file length (number of bytes)
// to be used to print the content
int get_file_length(int fd)
{
  int file_length = (int) lseek(fd, 0, SEEK_END);
  if (file_length == -1)
    throw_error(ERR_FILE_SEEK);

  // Go back to the start of the file
  if ((int) lseek(fd, 0, SEEK_SET) == -1)
    throw_error(ERR_FILE_SEEK);

  return file_length;
}

// Read file from file descriptor and file length
char *read_file(int fd, int file_length)
{
  // Allocate memory for file content
  char *buf = (char *) calloc(file_length, sizeof(char));

  // Read the file
  if (read(fd, buf, file_length) == -1)
    throw_error(ERR_FILE_READ);

  return buf;
}

// Close file from file descriptor
void close_file(int fd, char *buf)
{
  free(buf);
  if (close(fd) == -1)
    throw_error(ERR_FILE_CLOSE);
}

// Parse CSV row
void parse_line(char *line, char section)
{
  int student_id;
  int sum = 0, count = 0;
  char student_section;
  char *token;
  char *saveptr;
  char *delim = ",";

  // Student ID
  token = strtok_r(line, delim, &saveptr);
  student_id = atoi(token);

  // Student section
  token = strtok_r(NULL, delim, &saveptr);
  student_section = token[0];

  // If the student section is not same
  // as the requested section, return
  if (student_section != section) return;

  // Loop over next ints, calc sum and count
  while ((token = strtok_r(NULL, delim, &saveptr)))
  {
    sum += atoi(token);
    count++;
  }

  // Calculate average
  float average = (float) sum / count;

  // Assign memory for storing to-be-printed text
  char *text = (char *) calloc(20, sizeof(char));
  snprintf(text, 20, "%d: %f\n", student_id, average);

  // Write final output to stdout
  if (write(1, text, strlen(text)) == -1)
    throw_error(ERR_WRITE);

  free(text);
}

// Parse the CSV table
void parse_file(char *file_content, char section)
{
  char *saveptr;
  char *delim = "\n";
  char *line = strtok_r(file_content, delim, &saveptr);
  int header = 1;

  do {
    // Skip the initial CSV header
    if (header)
    {
      header = 0;
      continue;
    }

    // Parse the current line
    parse_line(line, section);
  }
  while ((line = strtok_r(NULL, delim, &saveptr)));
}

int main(int argc, char *argv[])
{
  // Fork the process and store child pid
  int child_pid = fork();

  if (child_pid < 0)
  { // Error in forking
    throw_error(ERR_FORK);
  }
  else if (child_pid == 0)
  { // Child process
    int fd = open_file();
    int file_length = get_file_length(fd);
    char *file_content = read_file(fd, file_length);

    printf("Section A:\n");
    parse_file(file_content, 'A');

    close_file(fd, file_content);
    exit(0);
  }
  else
  { // Parent process
    waitpid(child_pid, NULL, 0);

    int fd = open_file();
    int file_length = get_file_length(fd);
    char *file_content = read_file(fd, file_length);

    printf("\n\nSection B:\n");
    parse_file(file_content, 'B');

    close_file(fd, file_content);
  }

  return 0;
}
