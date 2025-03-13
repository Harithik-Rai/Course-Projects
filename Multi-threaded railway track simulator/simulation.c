#include <ctype.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#define MAX_TRAINS 75  // Max number of trains allowed as stated in question

pthread_mutex_t load_mutex, q_mutex, track_mutex;
pthread_cond_t start_load, load_done, load_ready, cross_done;

// Variables for tracking train states and time
int train_total = 0; 
struct timespec sim_start_time; 
int load_start_flag = 0; 
int queue_count = 0; 
int track_occupied = 0; 

// Linked list nodes for east/west trains
struct TrainNode *west_head = NULL;
struct TrainNode *east_head = NULL;

// Array to track the status of each train
enum TrainStatus *status_arr;

enum TrainStatus {
  WAITING,
  APPROVED
};

enum TrainPriority {
  LOW,
  HIGH
};

// Train structure
struct Train {
  int id;  // Train ID
  enum TrainPriority priority;  // HIGH or LOW
  pthread_t tid;
  char dest[5];  // East or West
  int load_duration;
  int cross_duration;
  pthread_cond_t cross_signal;
};

struct TrainNode {
  struct Train *train;  // Pointer to the train
  struct TrainNode *next;
  enum TrainPriority priority;  // Priority of the train (LOW or HIGH)
};

void get_current_time(struct timespec *time_spec) {
    if (clock_gettime(CLOCK_MONOTONIC, time_spec) != 0) {
        fprintf(stderr, "Error fetching time: ");
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }
}

// Function to convert into seconds
double get_time_sec(struct timespec *ts) {
    double seconds = (double)ts->tv_sec;
    double nanoseconds = (double)ts->tv_nsec;
    return seconds + (nanoseconds / 1e9);  // Combine and return total time in seconds (1e9=Nanosecond conversion)
}


int compare_train_priority(struct Train *t1, struct Train *t2) {
    if (t1->priority > t2->priority) {
        return 1;
    }
    if (t1->priority == t2->priority) {
        if (t1->load_duration == t2->load_duration && t1->id < t2->id) {
            return 1;
        }
    }

    return 0;
}

// Function to print simulation time in correct format like in assignment
void print_sim_time(struct timespec *start, struct timespec *end, char *msg) {
  double elapsed = get_time_sec(end) - get_time_sec(start);

  int hours = (int)(elapsed / 3600);
  int minutes = (int)((elapsed - hours * 3600) / 60);
  double seconds = elapsed - hours * 3600 - minutes * 60;
  printf("%02d:%02d:%04.1f %s", hours, minutes, seconds, msg);
}

void add_to_queue(struct Train *train, struct TrainNode **head) {
  struct TrainNode *new_node = malloc(sizeof(struct TrainNode));

  if (!new_node) {
    printf("Memory allocation failed\n");
    exit(0);
  }
  new_node->train = train;
  new_node->next = NULL;
  new_node->priority = train->priority;

  struct TrainNode *current = *head;
  if (!(*head)) {
    *head = new_node;
  } else if (compare_train_priority(new_node->train, (*head)->train)) {
    new_node->next = *head;
    *head = new_node;
  } else {
    while (current->next && !compare_train_priority(new_node->train, current->next->train)) {
      current = current->next;
    }

    new_node->next = current->next;
    current->next = new_node;
  }
}

int is_queue_empty(struct TrainNode **head) {
  return !(*head);
}

struct Train *get_first_train(struct TrainNode **head) {
  return (*head)->train;
}

struct Train *remove_from_queue(struct TrainNode **head) {
  struct TrainNode *temp = *head;
  struct Train *train = (*head)->train;
  *head = (*head)->next;
  free(temp);
  return train;
}

// Thread function for each train
void *train_thread_func(void *train_ptr) {
  struct Train *train = train_ptr;
  struct timespec thread_time;

  // Wait for the start signal
  pthread_mutex_lock(&load_mutex);
  train_total++;
  pthread_cond_signal(&load_ready);
  while (!load_start_flag) {
    pthread_cond_wait(&start_load, &load_mutex);
  }
  pthread_mutex_unlock(&load_mutex);

  // Simulate loading time
  usleep(train->load_duration * 100000.0);
  char msg[100];
  sprintf(msg, "Train %2d is ready to go %4s\n", train->id, train->dest);
  get_current_time(&thread_time);
  print_sim_time(&sim_start_time, &thread_time, msg);

  // Add train to the right queue
  pthread_mutex_lock(&q_mutex);
  if (strcmp(train->dest, "West") == 0) {
    add_to_queue(train, &west_head);
  } else {
    add_to_queue(train, &east_head);
  }
  queue_count++;
  pthread_cond_signal(&load_done);
  status_arr[train->id] = WAITING;
  pthread_mutex_unlock(&q_mutex);

  // Wait for permission to cross the track
  pthread_mutex_lock(&track_mutex);
  while (status_arr[train->id] != APPROVED) {
    pthread_cond_wait(&train->cross_signal, &track_mutex);
  }

  // Simulate crossing time
  sprintf(msg, "Train %2d is ON the main track going %4s\n", train->id, train->dest);
  get_current_time(&thread_time);
  print_sim_time(&sim_start_time, &thread_time, msg);
  usleep(train->cross_duration * 100000.0);
  sprintf(msg, "Train %2d is OFF the main track after going %4s\n", train->id, train->dest);
  get_current_time(&thread_time);
  print_sim_time(&sim_start_time, &thread_time, msg);

  // Signal that the train has crossed
  track_occupied = 0;
  pthread_cond_signal(&cross_done);
  pthread_mutex_unlock(&track_mutex);

  pthread_cond_destroy(&train->cross_signal);
  free(train);
  pthread_exit(NULL);
}

// Function to read the input file and create train threads
int read_input_file(char *filename) {
  FILE *file = fopen(filename, "r");

  if (!file) {
    printf("File not found\n");
    exit(1);
  }

  struct Train *train;
  char dir;
  int load, cross, err;
  int train_id = 0;
  enum TrainPriority prio;
  char *dest;

  while (fscanf(file, "%c %d %d\n", &dir, &load, &cross) != EOF) {
    if (train_id >= MAX_TRAINS) {
      printf("Error: Maximum number of trains (%d) exceeded.\n", MAX_TRAINS); // Max would be 75 as stated in question
      exit(1);
    }

    prio = isupper(dir) ? HIGH : LOW;
    dest = toupper(dir) == 'E' ? "East" : "West";
    train = malloc(sizeof(struct Train));
    train->id = train_id;
    train->priority = prio;
    strcpy(train->dest, dest);
    train->load_duration = load;
    train->cross_duration = cross;
    pthread_cond_init(&train->cross_signal, NULL);

    train_id++;
    err = pthread_create(&train->tid, NULL, train_thread_func, train);
    if (err) {
      printf("Thread creation failed: %d\n", err);
      exit(1);
    }
  }
  status_arr = malloc(sizeof(enum TrainStatus) * MAX_TRAINS);
  return train_id;
}

struct Train *remove_east_train(char *last, int *count) {
    if (*last == 'e') {
        (*count)++;
    } else {
        *count = 1;
    }

    *last = 'e';
    return remove_from_queue(&east_head);
}

struct Train *remove_west_train(char *last, int *count) {
    if (*last == 'w') {
        (*count)++;
    } else {
        *count = 1;
    }
    *last = 'w';
    return remove_from_queue(&west_head);
}


// Function that selects next train based on priority and rules given in assignment
struct Train *select_next_train(char *last, int *count) {
    struct Train *next_train = NULL;
    int west_priority = is_queue_empty(&west_head) ? -1 : get_first_train(&west_head)->priority;
    int east_priority = is_queue_empty(&east_head) ? -1 : get_first_train(&east_head)->priority;

    if (is_queue_empty(&west_head)) {
        next_train = remove_east_train(last, count);
    } else if (is_queue_empty(&east_head)) {
        next_train = remove_west_train(last, count);
    } else {
        if (*count >= 2) {
            // After two consecutive trains in the same direction, switch to the opposite direction
            if (*last == 'w') {
                next_train = remove_east_train(last, count);
            } else {
                next_train = remove_west_train(last, count);
            }
        } else {
            if (west_priority == HIGH && east_priority == LOW) {
                next_train = remove_west_train(last, count);
            } else if (west_priority == LOW && east_priority == HIGH) {
                next_train = remove_east_train(last, count);
            } else {
                if (*last == 'w') {
                    next_train = remove_east_train(last, count);
                } else {
                    next_train = remove_west_train(last, count);
                }
            }
        }
    }

    if (!next_train) {
        printf("No trains in queue\n");
    }

    return next_train;
}

int main(int argc, char *argv[]) {
  // Initialize
  pthread_mutex_init(&load_mutex, NULL);
  pthread_mutex_init(&q_mutex, NULL);
  pthread_mutex_init(&track_mutex, NULL);
  pthread_cond_init(&load_done, NULL);
  pthread_cond_init(&start_load, NULL);
  pthread_cond_init(&load_ready, NULL);
  pthread_cond_init(&cross_done, NULL);

  // Read input file and create the train threads
  int total_trains = read_input_file(argv[1]);

  // Wait for all trains to be ready
  pthread_mutex_lock(&load_mutex);
  while (train_total < total_trains) {
    pthread_cond_wait(&load_ready, &load_mutex);
  }
  load_start_flag = 1;
  pthread_cond_broadcast(&start_load);
  get_current_time(&sim_start_time);
  pthread_mutex_unlock(&load_mutex);

  // Variables to track the last direction and consecutive trains
  char last_direction = 'n';  // 'n' for none, 'w' for west, 'e' for east
  int consecutive_count = 0;
  int remaining_trains = total_trains;
  pthread_t train_threads[MAX_TRAINS];  // Array to store thread IDs

  while (remaining_trains > 0) {
    pthread_mutex_lock(&q_mutex);
    while (queue_count <= 0) {
      pthread_cond_wait(&load_done, &q_mutex);
    }
    struct Train *current_train = select_next_train(&last_direction, &consecutive_count);
    train_threads[current_train->id] = current_train->tid;
    queue_count--;
    remaining_trains--;
    track_occupied = 1;
    pthread_mutex_unlock(&q_mutex);

    // Give permission to cross
    pthread_mutex_lock(&track_mutex);
    status_arr[current_train->id] = APPROVED;
    pthread_mutex_unlock(&track_mutex);
    pthread_cond_signal(&current_train->cross_signal);

    // Wait for the train to finish crossing
    pthread_mutex_lock(&track_mutex);
    while (track_occupied) {
      pthread_cond_wait(&cross_done, &track_mutex);
    }
    pthread_mutex_unlock(&track_mutex);
  }

  // Wait for all train threads to finish
  for (int i = 0; i < total_trains; i++) {
    pthread_join(train_threads[i], NULL);
  }

  free(status_arr);
  pthread_mutex_destroy(&load_mutex);
  pthread_mutex_destroy(&q_mutex);
  pthread_mutex_destroy(&track_mutex);
  pthread_cond_destroy(&load_done);
  pthread_cond_destroy(&start_load);
  pthread_cond_destroy(&load_ready);
  pthread_cond_destroy(&cross_done);

  pthread_exit(NULL);
}
