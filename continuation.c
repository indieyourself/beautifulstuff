/**
	http://www.taika.org/~tomba/monoco/index.html
	http://1234n.com/?post/4vzsvm
	
	https://www.gnu.org/software/pth/pth-manual.html
	gcc -W continuation.c -o continuation
*/

#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>
#include <unistd.h>

typedef void (*handler)();

typedef struct Continuation
{
	ucontext_t *context;
	handler f;
	struct Continuation *next;
	int id;
	int state; // 0-init 1-running 2-blocked
} Continuation; 

Continuation *createContinuation( handler f , int id) {
	Continuation *continuation  = (Continuation *)malloc( sizeof(Continuation) );
	continuation -> f = f;
	continuation -> context = (ucontext_t *) malloc( sizeof(ucontext_t) );
	continuation -> context->uc_stack.ss_sp   = (char *) malloc(SIGSTKSZ);
   	continuation -> context->uc_stack.ss_size = SIGSTKSZ;
	continuation -> next = NULL;
	continuation -> id = id;
	continuation -> state = 0;

	return continuation;
}

void freeContinuation( Continuation *continuation ) {}

typedef struct ContinuQueue
{
	Continuation *head;
	Continuation *tail;
	int size;
} ContinuQueue;

ContinuQueue *createQueue( ) {
	ContinuQueue *queue = (ContinuQueue *)malloc( sizeof(ContinuQueue) );
	queue->head = NULL;
	queue->tail = NULL;
	queue->size = 0;

	return queue;
}

void enqueue( ContinuQueue *queue,  Continuation *node) {
	if (queue -> size == 0) {
		queue-> head= node;
		queue-> tail = node;
	} else {
		queue->tail->next = node;
		queue->tail = node;
	}
	
	++queue -> size;
}

Continuation *dequeue( ContinuQueue *queue ) {
	if ( queue -> size == 0) {
		return NULL;
	} else  {
		Continuation *tmp = queue->head;
		queue->head = tmp -> next;
		--queue -> size;

		return tmp;
	}
}

void freeQueue( ContinuQueue *queue ) {}

volatile Continuation *gmain;
volatile Continuation *gcurrent;
ContinuQueue *gqueue;

void switchcc(  Continuation *current,  Continuation *next ) {

	gcurrent = next;
	setcontext( gcurrent->context);
}

void init() {
	gcurrent = NULL;
	gqueue = createQueue();
}

void runContinuation(  ) {
	if (gcurrent->state == 0) {
		puts("get context");
		getcontext(gcurrent -> context);
	}

	if (gcurrent->state == 1) {
		gcurrent -> f();

		// return to main
		setcontext(gmain->context);
	}

	puts("runContinuation end");
}

void run() {
	getcontext(gmain->context);
	sleep(1);

	Continuation *continuation = dequeue(gqueue);
	printf("queuesize %d\n", gqueue->size);

	if (continuation == NULL) {
		puts("exit main run");
		return;
	} else {
		gcurrent = continuation;
		runContinuation();
		gcurrent -> state = 1;

		setcontext(gcurrent -> context);
	}
}

void schedule() {
	puts("schedule to main");
	setcontext(gmain->context);
}

/*
	test below
*/
void printH1( ) {
	puts("H1");
	int i = 0;
	
	while ( i< 3) {
		printf("%s %d\n", "H1", i);
		++i ;


		// memory management
		//Continuation *continuation = createContinuation(printH1, 1);
		//continuation -> state = 2;

		//free(gcurrent->context);
		//gcurrent->context =  (ucontext_t *) malloc( sizeof(ucontext_t) );
		gcurrent -> state = 2;
		enqueue(gqueue , gcurrent);
		
		//printf("i: %d \n",  i);
		//int result = getcontext( continuation->context );
		//printf("result: %d i: %d \n", result, i);
		//gcurrent = continuation;

		if ( gcurrent -> state  == 2) {
			puts("schedule h1");
			swapcontext( gcurrent->context, gmain->context);
			//schedule();
		}

		printf("re while %d\n", i);
	}

	puts("after a while");

	/*
	for( ; i < 10; ++i) {
		printf("%s %d\n", "H1", i);

		//Continuation *continuation = createContinuation

		// memory management
		free(gcurrent->context);
		gcurrent->context =  (ucontext_t *) malloc( sizeof(ucontext_t) );
		gcurrent -> state = 2;
		printf("id:  %d 1 -> 2\n", gcurrent->id);

		int result = getcontext(gcurrent->context);
		//printf("result: %d \n", result);
		//printf("state: %d \n", gcurrent->state);
		
		printf("i: %d \n", i);
		enqueue(gqueue , gcurrent);

		if ( gcurrent -> state  == 2) {
			puts("schedule h1");
			schedule();
		}

		puts("re for ");
	}*/
}

void printH2(  ) {
	puts("H2");
	int i = 0;
	for( i = 0; i < 3; ++i) {
		printf("%s %d\n", "H2", i);
	}
}


int main(void) {
	init();

	Continuation *c1 = createContinuation(printH1, 1);
	Continuation *c2 = createContinuation(printH2, 2);

	gmain = createContinuation( run, 0 );

	enqueue(gqueue, c1);
	enqueue(gqueue, c2);


	run();
	return 0;
}
