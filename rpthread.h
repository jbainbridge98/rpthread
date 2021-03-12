// File:	rpthread_t.h

// List all group member's name: Jackson Bainbridge Michael Rucando
// username of iLab:jdb343 mwr72
// iLab Server: plastic

#ifndef RTHREAD_T_H
#define RTHREAD_T_H


#define _GNU_SOURCE

/* To use Linux pthread Library in Benchmark, you have to comment the USE_RTHREAD macro */
#define USE_RTHREAD 1

#ifndef TIMESLICE
/* defined timeslice to 5 ms, feel free to change this while testing your code
 * it can be done directly in the Makefile*/
#define TIMESLICE 5
#endif

#define RUNNING 0
#define READY 1
#define SCHEDULED 2
#define BLOCKED 3
#define FINISHED 4

//added macros
#define STACKSIZE 8192

/* include lib header files that you need here: */
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>

typedef uint rpthread_t;

typedef struct threadControlBlock {
	/* add important states in a thread control block */
  uint id;
  int status;
  ucontext_t* tcontext;
  void* stackPtr;
  int priority;
  void* returnPtr;
} tcb;

/* mutex struct definition */
typedef struct rpthread_mutex_t {
	/* add something here */
  int locked;
	// YOUR CODE HERE
} rpthread_mutex_t;

/* define your data structures here: */
// Feel free to add your own auxiliary data structures (linked list or queue etc...)

// YOUR CODE HERE
typedef struct runqueue{
  tcb *threadBlock;
  int priority;
  struct runqueue *next;
}runqueue;

/* added global variables*/
int threadIDs;
ucontext_t* schedContext;
runqueue *runningThread;
runqueue *rqhead;
runqueue *array;
struct itimerval timer;
int yielded;
int exited;
int add_t;
runqueue *mlfqQueue;

/* Function Declarations: */

/* create a new thread */
int rpthread_create(rpthread_t * thread, pthread_attr_t * attr, void
    *(*function)(void*), void * arg);

/* give CPU pocession to other user level threads voluntarily */
int rpthread_yield();

/* terminate a thread */
void rpthread_exit(void *value_ptr);

/* wait for thread termination */
int rpthread_join(rpthread_t thread, void **value_ptr);

/* initial the mutex lock */
int rpthread_mutex_init(rpthread_mutex_t *mutex, const pthread_mutexattr_t
    *mutexattr);

/* aquire the mutex lock */
int rpthread_mutex_lock(rpthread_mutex_t *mutex);

/* release the mutex lock */
int rpthread_mutex_unlock(rpthread_mutex_t *mutex);

/* destroy the mutex */
int rpthread_mutex_destroy(rpthread_mutex_t *mutex);

void timerStart();

void timerStop();

void sigHandler(int signum);

void addThread(runqueue *head, runqueue *toAdd);

void deleteThread(runqueue *head, runqueue *toRemove);

static void sched_rr();

static void schedule();

void removeHead(runqueue *head);

void addArray(runqueue *head, runqueue *toAdd);

void printQueue();

void arrayPrint();

void addToMlfq(runqueue *head, runqueue *toAdd);

static void sched_mlfq();

#ifdef USE_RTHREAD
#define pthread_t rpthread_t
#define pthread_mutex_t rpthread_mutex_t
#define pthread_create rpthread_create
#define pthread_exit rpthread_exit
#define pthread_join rpthread_join
#define pthread_mutex_init rpthread_mutex_init
#define pthread_mutex_lock rpthread_mutex_lock
#define pthread_mutex_unlock rpthread_mutex_unlock
#define pthread_mutex_destroy rpthread_mutex_destroy
#endif

#endif
