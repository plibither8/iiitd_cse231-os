#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct entry
{
  char *name;
  long inode;
} entry;

typedef struct flags
{
  int all;
  int list;
  int inode;
} flags;

int ERR_STATUS = 0;

void print_entry(entry entry, flags flag)
{
  if (flag.list)
    if (flag.inode)
      printf("%ld %s\n", entry.inode, entry.name);
    else
      printf("%s\n", entry.name);
  else
    if (flag.inode)
      printf("%ld %s    ", entry.inode, entry.name);
    else
      printf("%s    ", entry.name);
}

int get_entry_count(char *path)
{
  int count = 0;

  DIR *active_dir = opendir(path);
  struct dirent *entry;
  while ((entry = readdir(active_dir)) != NULL)
    count++;

  closedir(active_dir);
  return count;
}

int check_file_exists_is_file(char *a_path, char *r_path, flags flag)
{
  struct stat f_status = { 0 };

  if(stat(a_path, &f_status) == -1)
  {
    fprintf(stderr, "ls: cannot access '%s': No such file or directory\n", r_path);
    ERR_STATUS = 1;
    return 1;
  }

  if (S_ISREG(f_status.st_mode))
  {
    if (!flag.list) printf("\n");
    print_entry((entry) { r_path, f_status.st_ino }, flag);
    return 1;
  }

  return 0;
}

int compare(const void *a, const void *b)
{
  return strcmp(((entry *) a)->name, ((entry *) b)->name);
}

void ls_directory(char *path, flags flag)
{
  DIR *active_dir = opendir(path);

  int entry_count = get_entry_count(path), active_entry_count = 0;
  entry curr_dir, parent_dir;
  entry entries[entry_count];
  struct dirent *raw_entry;

  while((raw_entry = readdir(active_dir)))
  {
    char *name = raw_entry->d_name;
    long inode = raw_entry->d_ino;

    entry refined_entry = { name, inode };

    if (strcmp(name, ".") == 0)
    {
      curr_dir = refined_entry;
      continue;
    }

    if (strcmp(name, "..") == 0)
    {
      parent_dir = refined_entry;
      continue;
    }

    if (!flag.all && name[0] == '.')
      continue;

    entries[active_entry_count++] = refined_entry;
  }

  closedir(active_dir);

  if (flag.all)
  {
    // First print the dot base dirs
    print_entry(curr_dir, flag);
    print_entry(parent_dir, flag);
  }

  if (active_entry_count > 0)
  {
    // Sort the dir and file name arrays
    qsort(entries, active_entry_count, sizeof(entries[0]), compare);

    // Then print rest of the entries
    for (int i = 0; i < active_entry_count; i++)
      print_entry(entries[i], flag);
  }

  free(path);
}

char *resolve_path(char *cwd, char *arg)
{
  char *new_path = (char *) calloc(200, sizeof(char));
  if (arg[0] == '/')
    strcpy(new_path, arg);
  else
  {
    strcpy(new_path, cwd);
    strcat(new_path, "/");
    strcat(new_path, arg);
  }
  return new_path;
}

int main(int argc, char *argv[])
{
  char *cwd = argv[0];

  char *a_paths[argc - 1];
  char *r_paths[argc - 1];
  int path_count = 0;
  flags flag = { 0, 0, 0 };

  for (int i = 2; i < argc; i++)
  {
    char *arg = argv[i];

    // Double dash keyword options
    if (strlen(arg) > 2 && arg[0] == '-' && arg[1] == '-')
    {
      if (strcmp(arg, "--all") == 0) flag.all = 1;
      else if (strcmp(arg, "--inode") == 0) flag.inode = 1;
      else
      {
        fprintf(stderr, "ls: unrecognized option '%s'\n", arg);
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
        if (letter == 'a') flag.all = 1;
        else if (letter == 'i') flag.inode = 1;
        else if (letter == '1') flag.list = 1;
        else
        {
          fprintf(stderr, "ls: invalid option -- '%c'\n", letter);
          exit(EXIT_FAILURE);
        }
      }
      continue;
    }

    // This argument is not an option/flag its a path to be ls'ed
    a_paths[path_count] = resolve_path(cwd, arg);
    r_paths[path_count++] = arg;
  }

  // If no path is provided in the args, we take the cwd
  if (path_count == 0)
  {
    a_paths[0] = resolve_path(cwd, "");
    r_paths[0] = "";
    path_count++;
  }

  if (path_count == 1)
  {
    char *a_path = a_paths[0];
    char *r_path = r_paths[0];
    if (!check_file_exists_is_file(a_path, r_path, flag))
      ls_directory(a_path, flag);
  }
  else
  {
    for (int i = 0; i < path_count; i++)
    {
      char *a_path = a_paths[i];
      char *r_path = r_paths[i];

      if (check_file_exists_is_file(a_path, r_path, flag))
      {
        printf("\n");
        continue;
      }

      if (i > 0 && !flag.list)
        printf("\n");
      printf("%s:\n", r_path);
      ls_directory(a_path, flag);
      printf("\n");
    }
  }

  return ERR_STATUS;
}
