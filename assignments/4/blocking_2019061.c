#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Duration for sleeping and waiting (eat duration also)
#define SLEEP_DURATION 1

// Our custom semaphore
typedef struct my_semaphore {
  pthread_mutex_t mutex; // the mutex which should be locked/release on edit the value
  pthread_cond_t conditional_var; // the conditional variable
  int max_value; // the maximum value of the counting semaphore
  int value; // the value of the semaphore, initially holding max value
} sem_t;

// Semaphore Initialising function
void my_semaphore_init(sem_t *S, int value) {
  S->value = S->max_value = value;
  pthread_cond_init(&S->conditional_var, NULL);
}

// Main wait function
void my_semaphore_wait(sem_t *S) {
  pthread_mutex_lock(&S->mutex);
  while (S->value == 0) // If available
    pthread_cond_wait(&S->conditional_var, &S->mutex);
  S->value--;
  pthread_mutex_unlock(&S->mutex);
}

// Main signal function
void my_semaphore_signal(sem_t *S) {
  pthread_mutex_lock(&S->mutex);
  // Checks
  if (S->value < S->max_value)
    S->value++;
  if (S->value >= 0)
    pthread_cond_signal(&S->conditional_var);
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

// Main thread function that makes the philosophers
// wait for forks/sauces
void *think_and_eat(void *__phil) {
  philosopher *phil = __phil;
  int left = phil->id;
  int right = (phil->id + 1) % phil_count;

  // Run forever
  while (1) {
    sleep(SLEEP_DURATION); // Think!
    my_semaphore_wait(&can_continue);

    // Wait for the forks
    my_semaphore_wait(&forks[left]);
    printf("P%d receives F%d\n", phil->id + 1, left);
    my_semaphore_wait(&forks[right]);
    my_semaphore_signal(&can_continue);

    // Wait for the sauce bowls
    my_semaphore_wait(&sauce_bowls);

    printf("P%d receives F%d,F%d\n", phil->id + 1, left, right);
    sleep(SLEEP_DURATION); // Eat!

    // Let go of forks and sauce bowls
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
