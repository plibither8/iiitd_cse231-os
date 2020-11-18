#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#define PHIL_COUNT 5
#define SLEEP_DURATION 1

sem_t can_continue;
sem_t forks[PHIL_COUNT];

struct philosopher {
  int id;
  int eaten;
  pthread_t thread;
};

struct philosopher philosophers[PHIL_COUNT];

void *think_and_eat(void *__phil) {
  struct philosopher *phil = __phil;
  int left = phil->id;
  int right = (phil->id + 1) % PHIL_COUNT;

  while (1) {
    // Break out of loop if all phils have eaten at least once
    int all_eaten = 1;
    for (int i = 0; i < PHIL_COUNT; i++) {
      if (!philosophers[i].eaten) {
        all_eaten = 0;
        break;
      }
    }
    if (all_eaten) break;

    sleep(SLEEP_DURATION); // Think!
    sem_wait(&can_continue);

    sem_wait(&forks[left]);
    printf("P%d receives F%d\n", phil->id + 1, left);

    sem_wait(&forks[right]);
    sem_post(&can_continue);

    printf("P%d receives F%d,F%d\n", phil->id + 1, left, right);
    phil->eaten = 1;
    sleep(SLEEP_DURATION); // Eat!

    sem_post(&forks[left]);
    sem_post(&forks[right]);
  }

  pthread_exit(0);
}

int main(int argc, char** argv) {
  // initialise the semaphores
  sem_init(&can_continue, 0, 1);

  for (int i = 0; i < PHIL_COUNT; i++) {
    sem_init(&forks[i], 0, 1);
  }

  // initialise the phil structs and threads
  for (int i = 0; i < PHIL_COUNT; i++) {
    philosophers[i].id = i;
    pthread_create(&(philosophers[i].thread), NULL, think_and_eat, &(philosophers[i]));
  }

  for (int i = 0; i < PHIL_COUNT; i++) {
    pthread_join(philosophers[i].thread, NULL);
  }

  return 0;
}
