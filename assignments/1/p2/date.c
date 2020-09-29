#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

typedef struct flags
{
  int rfc_email;
  int utc;
} flags;

int main(int argc, char *argv[])
{
  int operand_count = 0;
  char *date_format_string;

  flags flag = { 0, 0 };

  for (int i = 1; i < argc; i++)
  {
    char *arg = argv[i];

    // Double dash keyword options
    if (strlen(arg) > 2 && arg[0] == '-' && arg[1] == '-')
    {
      if (strcmp(arg, "--universal") == 0) flag.utc = 1;
      else if (strcmp(arg, "--utc") == 0) flag.utc = 1;
      else if (strcmp(arg, "--rfc-email") == 0) flag.rfc_email = 1;
      else
      {
        fprintf(stderr, "date: unrecognized option '%s'\n", arg);
        exit(EXIT_FAILURE);
      }
      continue;
    }

    // Single dash keyword letter options
    if (strlen(arg) > 1 && arg[0] == '-')
    {
      for (int j = 1; j < strlen(arg); j++)
      {
        char letter = arg[j];
        if (letter == 'u') flag.utc = 1;
        else if (letter == 'R') flag.rfc_email = 1;
        else
        {
          fprintf(stderr, "date: invalid option -- '%c'\n", letter);
          exit(EXIT_FAILURE);
        }
      }
      continue;
    }

    if (operand_count > 0)
    {
      fprintf(stderr, "date: extra operand '%s'\n", arg);
      exit(EXIT_FAILURE);
    }

    if (arg[0] != '+')
    {
      fprintf(stderr, "date: invalid date '%s'\n", arg);
      exit(EXIT_FAILURE);
    }

    date_format_string = &arg[1];
    operand_count++;
  }

  if (operand_count == 1 && flag.rfc_email)
  {
    fprintf(stderr, "date: multiple output formats specified\n");
    exit(EXIT_FAILURE);
  }

  time_t current_time = time(NULL);
  struct tm time_s = flag.utc ?
    *gmtime(&current_time) :
    *localtime(&current_time);

  if (operand_count == 0)
    date_format_string = "%a %d %b %T %Z %Y";

  if (flag.rfc_email)
    date_format_string = "%a, %d %b %Y %T %z";

  char *formatted_date = (char *) calloc(100, sizeof(char));
  strftime(formatted_date, 100, date_format_string, &time_s);
  printf("%s\n", formatted_date);

  free(formatted_date);
  return 0;
}
