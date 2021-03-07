// File:	rpthread.c

// List all group member's name:
// username of iLab:
// iLab Server:

#include "rpthread.h"

// INITAILIZE ALL YOUR VARIABLES HERE
// YOUR CODE HERE


/* create a new thread */
int rpthread_create(rpthread_t * thread, pthread_attr_t * attr, void *(*function)(void*), void * arg) {
       // create Thread Control Block
       // create and initialize the context of this thread
       // allocate space of stack for this thread to run
       // after everything is all set, push this thread int
       // YOUR CODE HERE
       //creating first thread
       if(schedContext == NULL){
         signal(SIGVTALRM, sigHandler);
         tcb* firstThread = (tcb*)malloc(sizeof(tcb));
         threadIDs = 0;
         firstThread->id = threadIDs;

         ucontext_t* firstContext = (ucontext_t*)malloc(sizeof(ucontext_t));

         if(getcontext(firstContext) == -1){
           printf("ERROR ON LINE 28\n");
         }


         firstThread->tcontext = firstContext;
         firstContext->uc_stack.ss_flags = 0;
         firstContext->uc_link = 0;

         //adding to runqueue, will also add to other runqueues for MLFQ once the scheduler structure is thought up
         threadIDs = 0;
         rqhead = malloc(sizeof(runqueue));
         rqhead->threadBlock = firstThread;
         runningThread = rqhead;
         /*firstThread->parent = NULL;
         firstThread->child = NULL;*/

         if(getcontext(schedContext) == -1){
           printf("ERROR ON LINE 44\n");
         }
         schedContext->uc_stack.ss_sp = malloc(STACKSIZE);
         schedContext->uc_stack.ss_size = STACKSIZE;
         schedContext->uc_stack.ss_flags = 0;

         if(sched == MLFQ){
           makecontext(schedContext, sched_mlfq, 0);
         }else if(sched == RR){
           makecontext(schedContext, sched_rr, 0);
         }
       }

       threadIDs++;
       tcb* newThread = (tcb*)malloc(sizeof(tcb));
       newThread->id = threadIDs;

       ucontext_t* newContext = (ucontext_t*)malloc(sizeof(ucontext_t));
       if(getcontext(newContext) == -1){
         printf("ERROR ON LINE 64\n");
       }

       newThread->tcontext = newContext;

       newContext->uc_stack.ss_sp = malloc(STACKSIZE);
       newContext->uc_stack.ss_size = STACKSIZE;
       newContext->uc_stack.ss_flags = 0;
       newContext->uc_link = 0;

       newThread->stackPtr = newContext->uc_stack.ss_sp;

       makecontext(newContext, (void*)function, 1, arg);

       runqueue *new = malloc(sizeof(runqueue));
       new->threadBlock = newThread;
       addThread(rqhead, newThread);
       newThread->status = READY;

    return 0;
};

/* give CPU possession to other user-level threads voluntarily */
int rpthread_yield() {

	// change thread state from Running to Ready
	// save context of this thread to its thread control block
	// wwitch from thread context to scheduler context

  //will add more when schedulers are done
  timerStop();
  runningThread->threadBlock->status = READY;
  // make a yeild flag so the scheduler knows it yeilded
  // its important for MLFQ
  // do the same for exit
  swapcontext(runningThread->threadBlock->tcontext, schedContext);

	// YOUR CODE HERE
	return 0;
};

/* terminate a thread */
void rpthread_exit(void *value_ptr) {
	// Deallocated any dynamic memory created when starting this thread

	// YOUR CODE HERE
  //will add more once schedulers are done
  timerStop();
  runningThread->threadBlock->status = FINISHED;
  runningThread->threadBlock->returnPtr = value_ptr;
  free(runningThread->threadBlock->stackPtr);
  swapcontext(runningThread->threadBlock->tcontext, schedContext);
};


/* Wait for thread termination */
int rpthread_join(rpthread_t thread, void **value_ptr) {

	// wait for a specific thread to terminate
	// de-allocate any dynamic memory created by the joining thread

	// YOUR CODE HERE
	return 0;
};

/* initialize the mutex lock */
int rpthread_mutex_init(rpthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr) {
	//initialize data structures for this mutex

	// YOUR CODE HERE
	return 0;
};

/* aquire the mutex lock */
int rpthread_mutex_lock(rpthread_mutex_t *mutex) {
        // use the built-in test-and-set atomic function to test the mutex
        // if the mutex is acquired successfully, enter the critical section
        // if acquiring mutex fails, push current thread into block list and //
        // context switch to the scheduler thread

        // YOUR CODE HERE
        return 0;
};

/* release the mutex lock */
int rpthread_mutex_unlock(rpthread_mutex_t *mutex) {
	// Release mutex and make it available again.
	// Put threads in block list to run queue
	// so that they could compete for mutex later.

	// YOUR CODE HERE
	return 0;
};


/* destroy the mutex */
int rpthread_mutex_destroy(rpthread_mutex_t *mutex) {
	// Deallocate dynamic memory created in rpthread_mutex_init

	return 0;
};

/* scheduler */
static void schedule() {
	// Every time when timer interrup happens, your thread library
	// should be contexted switched from thread context to this
	// schedule function

	// Invoke different actual scheduling algorithms
	// according to policy (RR or MLFQ)

	if (sched == RR){
	   sched_rr();
  }
  else if (sched == MLFQ){
	   sched_mlfq();
  }

	// YOUR CODE HERE

  // setup timer
  timerStart();
  // swap context here
  swapcontext(runningThread->threadBlock->tcontext, schedContext);

// schedule policy
#ifndef MLFQ
	// Choose RR
     // CODE 1
#else
	// Choose MLFQ
     // CODE 2
#endif

}

/* Round Robin (RR) scheduling algorithm */
static void sched_rr() {
	// Your own implementation of RR
	// (feel free to modify arguments and return types)

	// YOUR CODE HERE

  deleteThread(rqhead, runningThread);

  if (runningThread->threadBlock->status == READY){
    addThread(rqhead, runningThread);
  } else if (runningThread->status == FINISHED){
    //do nuffin
  } else if (runningThread->threadBlock->status == BLOCKED){
    addThread(rqhead, runningThread);
  } else {
    // error
  }

  /* start by getting the head of the runqueue
  set a virtual timer for whatever the timeslice is
  context switch to the thread */
}

/* Preemptive MLFQ scheduling algorithm */
static void sched_mlfq() {
	// Your own implementation of MLFQ
	// (feel free to modify arguments and return types)

	// YOUR CODE HERE

  //add runningThread
}

// Feel free to add any other functions you need
 void timerStart(){
   struct timeval it_interval;
   it_interval.tv_sec = 0;
   it_interval.tv_usec = 0;
   struct timeval it_value;
   it_value.tv_sec = 0;
   it_value.tv_usec = TIMESLICE;
   struct itimerval timer;
   timer.it_interval = it_interval;
   timer.it_value = it_value;
   setitimer(ITIMER_VIRTUAL, &timer, NULL);
 }

 void timerStop(){
   //struct timer slice;
   //timer.it_value = slice;
   struct timeval it_interval;
   it_interval.tv_sec = 0;
   it_interval.tv_usec = 0;
   struct timeval it_value;
   it_value.tv_sec = 0;
   it_value.tv_usec = 0;
   struct itimerval timer;
   timer.it_interval = it_interval;
   timer.it_value = it_value;
   setitimer(ITIMER_VIRTUAL, &timer, NULL);
 }

 void sigHandler(int signum){
   // do shit
   // swap context back to the scheduler
   runningThread->threadBlock->status = READY;
   swapcontext(runningThread->threadBlock->tcontext, schedContext);

 }

 void addThread(runqueue *head, runqueue *toAdd){
   while(head->next != NULL){
     head = head->next;
   }
   head->next = toAdd;
   toAdd->next == NULL;
 }

 void deleteThread(runqueue *head, runqueue *toRemove){
   runqueue *prev = head;
   while(head->threadBlock->id != toRemove->threadBlock->id && head->next != NULL){
     prev = head;
     head = head->next;
   }
   if(head->next == NULL && head->threadBlock->id == toRemove->threadBlock->id){
     runqueue *delete = head;
     free(head);
     prev->next == NULL;
   }
   if(head->threadBlock->id == toRemove->threadBlock->id){
     runqueue *next = head->next->next;
     free(head->next);
     head->next = next;
   }
 }
