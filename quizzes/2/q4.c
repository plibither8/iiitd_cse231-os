#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <fcntl.h>

#define KEY_INT 31415
#define KEY_CHAR 31416
#define SHM_PERMS 0666 | IPC_CREAT
#define MAX_STR_LEN 500
#define ITER_COUNT 10

int main(int argc, char** argv) {
  int shmid_int;
  int shmid_char;
  int *w_int;
  char *w_char;

  sem_unlink("write");
  sem_unlink("read");

  sem_t *sem_write = sem_open("write", O_CREAT, 0660, 0);
  sem_t *sem_read = sem_open("read", O_CREAT, 0660, 1);
  if (!sem_write || !sem_read) {
    perror("sem_open");
    exit(EXIT_FAILURE);
  }

  // Child process
  if (fork() == 0) {
    shmid_int = shmget(KEY_INT, sizeof(int), SHM_PERMS);
    shmid_char = shmget(KEY_CHAR, MAX_STR_LEN, SHM_PERMS);
    w_int = (int *) shmat(shmid_int, NULL, 0);
    w_char = (char *) shmat(shmid_char, NULL, 0);

    if (!w_int || !w_char) {
      perror("shmat");
      exit(EXIT_FAILURE);
    }

    // WRITING HERE
    for (int i = 0; i < ITER_COUNT; i++) {
      sem_wait(sem_read);
      strncpy(w_char, argv[1], MAX_STR_LEN);
      *w_int = atoi(argv[2]);
      sem_post(sem_write);
    }

    sem_wait(sem_read);
    strncpy(w_char, "$END$", MAX_STR_LEN);
    sem_post(sem_write);
  }
  // Parent process
  else {
    shmid_int = shmget(KEY_INT, sizeof(int), SHM_PERMS);
    shmid_char = shmget(KEY_CHAR, MAX_STR_LEN, SHM_PERMS);
    w_int = (int *) shmat(shmid_int, NULL, 0);
    w_char = (char *) shmat(shmid_char, NULL, 0);

    if (!w_int || !w_char) {
      perror("shmat");
      exit(EXIT_FAILURE);
    }

    // READING HERE
    while (1) {
      sem_wait(sem_write);
      if (!strcmp(w_char, "$END$")) {
        sem_post(sem_read);
        break;
      }

      printf("%s\n", w_char);
      printf("%d\n", *w_int);
      sem_post(sem_read);
    }
  }

  // Cleaning up
  sem_close(sem_write);
  sem_close(sem_read);

  if (shmdt(w_int) < 0 || shmdt(w_char) < 0) {
    perror("shmdt");
    exit(EXIT_FAILURE);
  }
  shmctl(KEY_INT, IPC_RMID, NULL);
  shmctl(KEY_CHAR, IPC_RMID, NULL);

  return 0;
}
