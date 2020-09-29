#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

typedef struct flags
{
  int no_newline;
  int disable_interpret;
} flags;

int echo(int argc, char *argv[])
{
  char *tokens[argc];
  int token_count = 0;
  int parse_options = 1;
  flags flag = { 0, 0 };

  for (int i = 0; i < argc; i++)
  {
    char *arg = argv[i];

    // Single dash keyword letter options
    if (parse_options && strlen(arg) > 1 && arg[0] == '-')
    {
      int is_token = 0;
      flags buffer = { 0, 0 };
      for (int j = 1; j < strlen(arg); j++)
      {
        char letter = arg[j];
        if (letter == 'n') buffer.no_newline = 1;
        else if (letter == 'e') buffer.disable_interpret = 1;
        else {
          is_token = 1;
          buffer = (flags) { 0, 0 };
          break;
        }
      }
      flag.no_newline |= buffer.no_newline;
      flag.disable_interpret |= buffer.disable_interpret;
      if (!is_token)
        continue;
    }

    // This argument is not an option/flag its a path to be cat'ed
    tokens[token_count++] = arg;
    parse_options = 0;
  }

  for (int i = 0; i < token_count; i++)
    printf("%s ", tokens[i]);

  if (!flag.no_newline)
    printf("\n");

  return 0;
}
