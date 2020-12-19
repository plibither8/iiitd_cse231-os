#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Duration for sleeping and waiting (eat duration also)
#define SLEEP_DURATION 1

// Our custom semaphore
typedef struct my_semaphore {
  pthread_mutex_t mutex; // the mutex which should be locked/release on edit the value
  int max_value; // the maximum value of the counting semaphore
  int value; // the value of the semaphore, initially holding max value
} sem_t;

// Semaphore Initialising function
void my_semaphore_init(sem_t *S, int value) {
  S->value = S->max_value = value;
}

// Main wait function
int my_semaphore_wait(sem_t *S) {
  if (pthread_mutex_trylock(&S->mutex)) return 0;
  if (S->value > 0) {
    S->value--;
    pthread_mutex_unlock(&S->mutex);
    return 1;
  } else {
    pthread_mutex_unlock(&S->mutex);
    return 0;
  }
}

// Main signal function
void my_semaphore_signal(sem_t *S) {
  pthread_mutex_lock(&S->mutex);
  if (S->value < S->max_value)
    S->value++;
  pthread_mutex_unlock(&S->mutex);
}

// Print for debugging
void my_semaphore_print(sem_t *S) {
  printf("%d\n", S->value);
}

// Philosopher count as argument
int phil_count;

// Our semaphores
sem_t can_continue;
sem_t *forks;
sem_t sauce_bowls;

// Philosopger struct containing id and phil thread
typedef struct philosopher {
  int id;
  pthread_t thread;
} philosopher;

philosopher *philosophers;

void *think_and_eat(void *__phil) {
  philosopher *phil = __phil;
  int left = phil->id;
  int right = (phil->id + 1) % phil_count;

  while (1) {
    sleep(SLEEP_DURATION); // Think!

    while (!my_semaphore_wait(&can_continue)) {} // wait

    // Wait for the forks
    while (!my_semaphore_wait(&forks[left])) {} // wait
    printf("P%d receives F%d\n", phil->id + 1, left);
    while (!my_semaphore_wait(&forks[right])) {} // wait
    my_semaphore_signal(&can_continue);

    while (!my_semaphore_wait(&sauce_bowls)) {} // wait

    // Wait for the sauce bowls

    printf("P%d receives F%d,F%d\n", phil->id + 1, left, right);
    sleep(SLEEP_DURATION); // Eat!

    my_semaphore_signal(&forks[left]);
    my_semaphore_signal(&forks[right]);
    my_semaphore_signal(&sauce_bowls);
  }

  pthread_exit(0);
}

int main(int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr, "Enter number of philosophers as argument");
    return 1;
  }

  phil_count = atoi(argv[1]);

  // initialise the semaphores
  my_semaphore_init(&can_continue, 1);
  my_semaphore_init(&sauce_bowls, 1);

  forks = (sem_t *) malloc(phil_count * sizeof(sem_t));
  philosophers = (philosopher *) malloc(phil_count * sizeof(philosopher));

  for (int i = 0; i < phil_count; i++) {
    my_semaphore_init(&forks[i], 1);
  }

  // initialise the phil structs and threads
  for (int i = 0; i < phil_count; i++) {
    philosophers[i].id = i;
    pthread_create(&(philosophers[i].thread), NULL, think_and_eat, &(philosophers[i]));
  }

  for (int i = 0; i < phil_count; i++) {
    pthread_join(philosophers[i].thread, NULL);
  }

  return 0;
}
