// File:	rpthread.c

// List all group member's name:Jackson Bainbridge Michael Rucando
// username of iLab:jdb343 mwr72
// iLab Server: plastic

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

         // scheduler doesn't exist, initialize

         schedContext = (ucontext_t*)malloc(sizeof(ucontext_t));
         if(getcontext(schedContext) == -1){
           printf("ERROR ON LINE 44\n");
         }
         schedContext->uc_stack.ss_sp = malloc(STACKSIZE);
         schedContext->uc_stack.ss_size = STACKSIZE;
         schedContext->uc_stack.ss_flags = 0;
         schedContext->uc_link = 0;
         makecontext(schedContext, schedule, 0);

         // initialize signal handler

         signal(SIGVTALRM, sigHandler);

         // create thread for main

         runqueue *mainThread = (runqueue*)malloc(sizeof(runqueue));
         runqueue *mainThreadArray = (runqueue*)malloc(sizeof(runqueue));
         mainThread->threadBlock = (tcb*)malloc(sizeof(tcb));
         mainThread->threadBlock->status = RUNNING;
         mainThread->threadBlock->id = 0;
         mainThread->threadBlock->tcontext = (ucontext_t*)malloc(sizeof(ucontext_t));
         mainThreadArray->threadBlock = mainThread->threadBlock;
         mainThread->threadBlock->priority = 3;
         mainThreadArray->threadBlock->priority = 3;
         threadIDs++;

         addThread(rqhead, mainThread);
         addArray(array, mainThreadArray);

         runningThread = mainThread;

         add_t = 1;

       }

       runqueue *newThread = (runqueue*)malloc(sizeof(runqueue));
       runqueue *newThreadArray = (runqueue*)malloc(sizeof(runqueue));
       newThread->threadBlock = (tcb*)malloc(sizeof(tcb));
       newThread->threadBlock->status = READY;
       newThread->threadBlock->id = *thread;

       ucontext_t* newContext = (ucontext_t*)malloc(sizeof(ucontext_t));
       getcontext(newContext);

       newThread->threadBlock->tcontext = newContext;
       newThread->threadBlock->tcontext->uc_stack.ss_sp = malloc(STACKSIZE);
       newThread->threadBlock->tcontext->uc_stack.ss_size = STACKSIZE;
       newThread->threadBlock->tcontext->uc_stack.ss_flags = 0;
       newThread->threadBlock->tcontext->uc_link = 0;
       newThread->threadBlock->id = threadIDs; *thread = (rpthread_t)threadIDs;
       threadIDs++;
       makecontext(newThread->threadBlock->tcontext, (void*)function, 1, arg);

       newThreadArray->threadBlock = newThread->threadBlock;

       newThread->threadBlock->priority = 3;
       newThreadArray->threadBlock->priority = 3;

       addThread(rqhead, newThread);
       addArray(array, newThreadArray);


       if (add_t) timerStart();

       return 0;
};

/* give CPU possession to other user-level threads voluntarily */
int rpthread_yield() {


	// change thread state from Running to Ready
	// save context of this thread to its thread control block
	// wwitch from thread context to scheduler context

  timerStop();
  runningThread->threadBlock->status = READY;
  // make a yeild flag so the scheduler knows it yeilded
  // its important for MLFQ
  // do the same for exit


  swapcontext(runningThread->threadBlock->tcontext, schedContext);

	return 0;
};

/* terminate a thread */
void rpthread_exit(void *value_ptr) {
	// Deallocated any dynamic memory created when starting this thread

	// YOUR CODE HERE
  timerStop();
  runningThread->threadBlock->status = FINISHED;
  runningThread->threadBlock->returnPtr = value_ptr;
  free(runningThread->threadBlock->tcontext->uc_stack.ss_sp);
  swapcontext(runningThread->threadBlock->tcontext, schedContext);

};


/* Wait for thread termination */
int rpthread_join(rpthread_t thread, void **value_ptr) {

	// wait for a specific thread to terminate
	// de-allocate any dynamic memory created by the joining thread

  runqueue *cur;
  cur = array;
  while (cur->threadBlock->id != thread){
    cur = cur->next;
  }
  while (cur->threadBlock->status != FINISHED){
    // wait
    rpthread_yield();
  }

  if (value_ptr != NULL){
    value_ptr = &cur->threadBlock->returnPtr;
  }

	return 0;
};

/* initialize the mutex lock */
int rpthread_mutex_init(rpthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr) {
	//initialize data structures for this mutex
  mutex->locked = 0;
	return 0;
};

/* aquire the mutex lock */
int rpthread_mutex_lock(rpthread_mutex_t *mutex) {
        // use the built-in test-and-set atomic function to test the mutex
        // if the mutex is acquired successfully, enter the critical section
        // if acquiring mutex fails, push current thread into block list and //
        // context switch to the scheduler thread

        while (mutex->locked == 1){
          rpthread_yield();
        }
        mutex->locked = 1;

        return 0;
};

/* release the mutex lock */
int rpthread_mutex_unlock(rpthread_mutex_t *mutex) {
	// Release mutex and make it available again.
	// Put threads in block list to run queue
	// so that they could compete for mutex later.

  mutex->locked = 0;

	return 0;
};


/* destroy the mutex */
int rpthread_mutex_destroy(rpthread_mutex_t *mutex) {
	// Deallocate dynamic memory created in rpthread_mutex_init

  //our mutexes are only ints, nothing to free

	return 0;
};

/* scheduler */
static void schedule() {
	// Every time when timer interrup happens, your thread library
	// should be contexted switched from thread context to this
	// schedule function
	// Invoke different actual scheduling algorithms
	// according to policy (RR or MLFQ)

#ifndef MLFQ
  	// Choose RR
sched_rr();
#else
  	// Choose MLFQ
sched_mlfq();
#endif

  // setup timer
  timerStart();
  // swap context here

  runningThread = rqhead;
  setcontext(rqhead->threadBlock->tcontext);

}

/* Round Robin (RR) scheduling algorithm */
static void sched_rr() {
	// Your own implementation of RR
	// (feel free to modify arguments and return types)

  rqhead = rqhead->next;

  runningThread->next = NULL;

  if (runningThread->threadBlock->status == READY){
    addThread(rqhead, runningThread);
  } else if (runningThread->threadBlock->status == FINISHED){
    //do nothing
  } else if (runningThread->threadBlock->status == BLOCKED){
    addThread(rqhead, runningThread);
  } else {
    //error
  }

  /* start by getting the head of the runqueue
  set a virtual timer for whatever the timeslice is
  context switch to the thread */
}

/* Preemptive MLFQ scheduling algorithm */
static void sched_mlfq() {
	// Your own implementation of MLFQ
	// (feel free to modify arguments and return types)

  rqhead = rqhead->next;

  runningThread->next = NULL;

  if (runningThread->threadBlock->status == READY){
    addToMlfq(rqhead, runningThread);
  } else if (runningThread->threadBlock->status == FINISHED){
    //do nothing
  } else if (runningThread->threadBlock->status == BLOCKED){
    addToMlfq(rqhead, runningThread);
  } else {
    //error
  }


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

   #ifndef MLFQ
   	// Choose RR
     // do nothing
   #else
   	// Choose MLFQ
    if (runningThread->threadBlock->priority > 0){
      runningThread->threadBlock->priority--;
    }
   #endif

   runningThread->threadBlock->status = READY;

   getcontext(runningThread->threadBlock->tcontext);
   swapcontext(runningThread->threadBlock->tcontext, schedContext);

 }

void removeHead(runqueue *head){

  head = head->next;

}

 void addThread(runqueue *head, runqueue *toAdd){

   if (head == NULL){
     rqhead = toAdd;
     rqhead->next == NULL;
     runningThread = toAdd;
     return;
   }
   while(head->next != NULL){
     head = head->next;
   }
   head->next = toAdd;
   toAdd->next == NULL;
 }

 void addArray(runqueue *head, runqueue *toAdd){

   if (head == NULL){
     array = toAdd;
     return;
   }
   while(head->next != NULL){
     head = head->next;
   }
   head->next = toAdd;
   toAdd->next == NULL;
 }

 void addToMlfq(runqueue *head, runqueue *toAdd){

   if(head == NULL){
     rqhead = toAdd;
     return;
   }
   while(head->next != NULL && head->next->threadBlock->priority > toAdd->threadBlock->priority){
     head = head->next;
   }
   if(head->next == NULL){
     head->next = toAdd;
     toAdd->next == NULL;
   }else{
     runqueue *temp;
     temp = head->next;
     head->next = toAdd;
     toAdd->next = temp;
   }

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

 void printQueue(){
   runqueue *cur;
   cur = rqhead;
   int count = 0;
   while(cur != NULL){
     count++;
     cur = cur->next;
   }
   printf("Number of entires : %d\n", count);
 }

void arrayPrint(){
  runqueue *cur;
  cur = array;
  while(cur != NULL){
    printf("THREAD: %d, STATUS: %d\n",cur->threadBlock->id, cur->threadBlock->status);
    cur = cur->next;
  }
}
