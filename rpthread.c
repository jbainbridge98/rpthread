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

       //puts("rpthread_create");

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
       makecontext(newThread->threadBlock->tcontext, (void*)function, 0, arg);

       newThreadArray->threadBlock = newThread->threadBlock;

       addThread(rqhead, newThread);
       addArray(array, newThreadArray);

       //arrayPrint();

       if (add_t) timerStart();

       return 0;
};

/* give CPU possession to other user-level threads voluntarily */
int rpthread_yield() {

  //puts("rpthread_yield");

	// change thread state from Running to Ready
	// save context of this thread to its thread control block
	// wwitch from thread context to scheduler context

  //puts("Yielded");

  //will add more when schedulers are done
  timerStop();
  runningThread->threadBlock->status = READY;
  // make a yeild flag so the scheduler knows it yeilded
  // its important for MLFQ
  // do the same for exit
  //puts("Got here");
  //if (swapcontext(runningThread->threadBlock->tcontext, schedContext)) puts("Fuck");

  swapcontext(runningThread->threadBlock->tcontext, schedContext);

	// YOUR CODE HERE
	return 0;
};

/* terminate a thread */
void rpthread_exit(void *value_ptr) {
	// Deallocated any dynamic memory created when starting this thread

  //puts("rpthread_exit");

	// YOUR CODE HERE
  //will add more once schedulers are done
  timerStop();
  runningThread->threadBlock->status = FINISHED;
  runningThread->threadBlock->returnPtr = value_ptr;
  free(runningThread->threadBlock->tcontext->uc_stack.ss_sp);
  //arrayPrint();
  swapcontext(runningThread->threadBlock->tcontext, schedContext);

};


/* Wait for thread termination */
int rpthread_join(rpthread_t thread, void **value_ptr) {

  //puts("rpthread_join");

	// wait for a specific thread to terminate
	// de-allocate any dynamic memory created by the joining thread

  //printQueue();

  runqueue *cur;
  cur = array;
  //if (array == NULL) puts("no good");
  //printf("TEST1\n");
  while (cur->threadBlock->id != thread){
    cur = cur->next;
    //printf("TEST2\n");
  }
  //printf("TEST3\n");
  while (cur->threadBlock->status != FINISHED){
    // wait
    //printf("TEST4\n");
  }

  if (value_ptr != NULL){
    value_ptr = &cur->threadBlock->returnPtr;
    //printf("TEST5\n");
  }

  //puts("other thread has finished");

	// YOUR CODE HERE
	return 0;
};

/* initialize the mutex lock */
int rpthread_mutex_init(rpthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr) {
	//initialize data structures for this mutex

  //puts("rpthread_mutex_init");

	// YOUR CODE HERE
	return 0;
};

/* aquire the mutex lock */
int rpthread_mutex_lock(rpthread_mutex_t *mutex) {
        // use the built-in test-and-set atomic function to test the mutex
        // if the mutex is acquired successfully, enter the critical section
        // if acquiring mutex fails, push current thread into block list and //
        // context switch to the scheduler thread

        //puts("rpthread_mutex_lock");

        // YOUR CODE HERE
        return 0;
};

/* release the mutex lock */
int rpthread_mutex_unlock(rpthread_mutex_t *mutex) {
	// Release mutex and make it available again.
	// Put threads in block list to run queue
	// so that they could compete for mutex later.

  //puts("rpthread_mutex_unlock");

	// YOUR CODE HERE
	return 0;
};


/* destroy the mutex */
int rpthread_mutex_destroy(rpthread_mutex_t *mutex) {
	// Deallocate dynamic memory created in rpthread_mutex_init

  //puts("rpthread_mutex_destroy");

	return 0;
};

/* scheduler */
static void schedule() {
	// Every time when timer interrup happens, your thread library
	// should be contexted switched from thread context to this
	// schedule function

  //puts("schedule");

  //puts("Sched invoked");

  //puts("We did it!");

	// Invoke different actual scheduling algorithms
	// according to policy (RR or MLFQ)

	//if (sched == RR){
	   sched_rr();
  //}
  //else if (sched == MLFQ){
	  // sched_mlfq();
  //}

	// YOUR CODE HERE

  // setup timer
  timerStart();
  //puts("Started timer");
  // swap context here
  //puts("exiting scheduler");

  runningThread = rqhead;
  setcontext(rqhead->threadBlock->tcontext);

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

  //puts("sched_rr");

  //removeHead(rqhead);

  rqhead = rqhead->next;

  runningThread->next = NULL;
  //puts("head removed");

  //puts("Entering if");

  if (runningThread->threadBlock->status == READY){
    //puts("adding back to queue");
    addThread(rqhead, runningThread);
  } else if (runningThread->threadBlock->status == FINISHED){
    //do nuffin
  } else if (runningThread->threadBlock->status == BLOCKED){
    addThread(rqhead, runningThread);
  } else {
    //printf("TEST in .c\n");
    //error
  }

  //puts("Thread added back into the queue");

  /* start by getting the head of the runqueue
  set a virtual timer for whatever the timeslice is
  context switch to the thread */
}

/* Preemptive MLFQ scheduling algorithm */
static void sched_mlfq() {
	// Your own implementation of MLFQ
	// (feel free to modify arguments and return types)

  //puts("sched_mlfq");

	// YOUR CODE HERE

  //add runningThread
}

// Feel free to add any other functions you need
 void timerStart(){

   //puts("timerStart");

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

   //puts("timerStop");

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

   //puts("sigHandler");

   // do shit
   // swap context back to the scheduler

   runningThread->threadBlock->status = READY;

   //puts("test");

   //if (runningThread->threadBlock->tcontext == NULL) puts("WHY");

   getcontext(runningThread->threadBlock->tcontext);
   swapcontext(runningThread->threadBlock->tcontext, schedContext);
   //puts("Failed");

 }

void removeHead(runqueue *head){

  //puts("removeHead");

  head = head->next;

}

 void addThread(runqueue *head, runqueue *toAdd){

   //puts("addThread");

   if (head == NULL){
     rqhead = toAdd;
     rqhead->next == NULL;
     runningThread = toAdd;
     //puts("Creating new head");
     return;
   }
   while(head->next != NULL){
     head = head->next;
   }
   head->next = toAdd;
   toAdd->next == NULL;
   //puts("Added thread to queue");
 }

 void addArray(runqueue *head, runqueue *toAdd){

   //puts("addArray");

   if (head == NULL){
     array = toAdd;
     //puts("Creating new head");
     return;
   }
   while(head->next != NULL){
     head = head->next;
   }
   head->next = toAdd;
   toAdd->next == NULL;
   //puts("Added thread to queue");
 }

 void deleteThread(runqueue *head, runqueue *toRemove){

   //puts("deleteThread");

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
