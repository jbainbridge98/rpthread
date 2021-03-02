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
       if(runqueue == NULL){
         tcb* firstThread = (tcb*)malloc(sizeof(tcb));
         threadIDs = 0;
         firstThread->id = threadIDs;

         ucontext_t* firstContext = (ucontext_t*)malloc(sizeof(ucontext_t));

         if(getcontext(&firstContext) == -1){
           printf("ERROR ON LINE 28\n");
         }


         firstThread->context = firstContext;
         firstContext->uc_stack.ss_flags = 0;
         firstContext->uc_link = 0;
         first

         //adding to runqueue, will also add to other runqueues for MLFQ once the scheduler structure is thought up

         rq = firstThread;
         firstThread->parent = NULL;
         firstThread->child = NULL;

         if(getcontext(&schedContext) == -1){
           printf("ERROR ON LINE 44\n");
         }
         schedContext.uc_stack.ss_sp = malloc(STACKSIZE);
         schedContext.uc_stack.ss_size = STACKSIZE;
         schedContext.uc_stack.ss_flags = 0;

         if(MLFQ){
           makecontext(&schedContext, sched_mlfq, 0);
         }else{
           makecontext(&schedContext, sched_rr, 0);
         }

       }

       threadIDs++;
       tcb* newThread = (tcb*)malloc(sizeof(tcb));
       newThread->id = threadIDs;

       ucontext_t* newContext = (ucontext_t*)malloc(sizeof(ucontext_t));
       getcontext(newContext);

       newThread->context = newContext;

       newContext->uc_stack.ss_sp = malloc(STACKSIZE);
       newContext->uc_stack.ss_size = STACKSIZE;
       newContext->uc_stack.ss_flags = 0;
       newContext->uc_link = 0;

       newThread->stackPtr = newContext->uc_stack.ss_sp;

       makecontext(newContext, (void*)function, 1, arg);

       while(rq->next != NULL){
         rq = rq->next;
       }
       rq->next = newThread;
       newThread->status = READY;

    return 0;
};

/* give CPU possession to other user-level threads voluntarily */
int rpthread_yield() {

	// change thread state from Running to Ready
	// save context of this thread to its thread control block
	// wwitch from thread context to scheduler context

  //will add more when schedulers are done

  runningThread->status = READY;
  swapcontext(runningThread->context, & schedContext);

	// YOUR CODE HERE
	return 0;
};

/* terminate a thread */
void rpthread_exit(void *value_ptr) {
	// Deallocated any dynamic memory created when starting this thread

	// YOUR CODE HERE
  //will add more once schedulers are done

  runningThread->status = FINISHED;
  //add something to give us back values i think???
  free(runningThread->stackPtr);
  swapcontext(runningThread->context, &schedContext);
};


/* Wait for thread termination */
int rpthread_join(rpthread_t thread, void **value_ptr) {

	// wait for a specific thread to terminate
	// de-allocate any dynamic memory created by the joining thread

	// YOUR CODE HERE
	return 0;
};

/* initialize the mutex lock */
int rpthread_mutex_init(rpthread_mutex_t *mutex,
                          const pthread_mutexattr_t *mutexattr) {
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

	// if (sched == RR)
	//		sched_rr();
	// else if (sched == MLFQ)
	// 		sched_mlfq();

	// YOUR CODE HERE

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

  //add runingThread
}

/* Preemptive MLFQ scheduling algorithm */
static void sched_mlfq() {
	// Your own implementation of MLFQ
	// (feel free to modify arguments and return types)

	// YOUR CODE HERE

  //add runningThread
}

// Feel free to add any other functions you need
/*void enqueue(tcb* head, tcb* toAdd){
  tcb* temp = head;
  while(temp->down != NULL){
    temp = temp->down;
  }
  temp->down = toAdd;
  toAdd->up = temp;
  toAdd->down = NULL;
}
*/
// YOUR CODE HERE
