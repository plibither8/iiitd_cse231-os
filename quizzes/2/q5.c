#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>

#define READER_COUNT 5
#define ITER_COUNT 10
#define MAP_ANONYMOUS 0x20
#define TOTAL_SIZE 8 // bytes
#define READ_SLEEP 1
#define WRITE_SLEEP 2
#define STEP_SIZE 100

sem_t write_sem;
pthread_mutex_t mutex;
int active_reader_count = 0;
int curr_write_iteration = 0;
long long *data;

void *read_data(void *__id) {
  int id = *((int *) __id);

  while (1) {
    sleep(READ_SLEEP);

    pthread_mutex_lock(&mutex);
    active_reader_count++;
    // Reading from all the reader threads has started
    // We will pause writing for the time being
    if (active_reader_count == 1) {
      sem_wait(&write_sem);
    }
    pthread_mutex_unlock(&mutex);

    // READ HERE
    printf("Reader %d: %lld\n", id, *data);

    pthread_mutex_lock(&mutex);
    active_reader_count--;
    // All reader threads have done reading, we can move
    // on to writing again.
    if (!active_reader_count) {
      sem_post(&write_sem);
    }
    pthread_mutex_unlock(&mutex);

    if (curr_write_iteration >= ITER_COUNT) {
      break;
    }
  }

  pthread_exit(0);
}

void *write_data(void *args) {
  while (curr_write_iteration < ITER_COUNT) {
    sleep(WRITE_SLEEP);

    sem_wait(&write_sem);
    *data += STEP_SIZE;
    curr_write_iteration++;
    sem_post(&write_sem);
  }

  pthread_exit(0);
}

int main(int argc, char** argv) {
  // Set up global data variable
  data = (long long *) mmap(NULL,
    TOTAL_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

  // The write semaphore and mutex
  sem_init(&write_sem, 0, 1);
  pthread_mutex_init(&mutex, NULL);

  // Set up writer thread
  pthread_t writer;
  pthread_create(&writer, NULL, write_data, NULL);

  // Set up reader threads
  pthread_t readers[READER_COUNT];
  for (int i = 0; i < READER_COUNT; i++) {
    pthread_create(&readers[i], NULL, read_data, &i);
  }

  pthread_join(writer, NULL);
  for (int i = 0; i < READER_COUNT; i++) {
    pthread_join(readers[i], NULL);
  }

  sem_destroy(&write_sem);
  pthread_mutex_destroy(&mutex);

  return 0;
}
