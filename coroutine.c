/*
	http://blog.chinaunix.net/uid-317451-id-92632.html
	https://gist.github.com/indieyourself/9c6a0b37b58d9d8d457c8c9ee06b2613
	
	http://blog.csdn.net/qq910894904/article/details/41911175

	gcc -W coroutine.c -o coroutine
*/
#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>
#include <unistd.h>

typedef void (*Handler)(void);

typedef struct Coroutine
{
	ucontext_t *context;
	Handler f;
	struct Coroutine *next;
	int state; // 0-init 1-running 2-blocked
	void *env;
} Coroutine; 

Coroutine *gmain;
volatile Coroutine *gcurrent;

Coroutine *createCoroutine( Handler callback, void *env, Coroutine *ret) {

	Coroutine *coroutine  = (Coroutine *)malloc( sizeof(Coroutine) );
	coroutine -> context = (ucontext_t *) malloc( sizeof(ucontext_t) );
	getcontext( coroutine -> context );

	coroutine -> f = callback;
	if ( ret == NULL) {
		coroutine -> context -> uc_link = NULL;
	} else {
		coroutine -> context -> uc_link = ret->context;
	}
	
	coroutine -> context->uc_stack.ss_sp   = (char *) malloc(SIGSTKSZ);
   	coroutine -> context->uc_stack.ss_size = SIGSTKSZ;
	coroutine -> next = NULL;
	coroutine -> env = env;
	coroutine -> state = 0;

	makecontext( coroutine->context, coroutine->f, 1, coroutine->env);

	return coroutine;
}

typedef struct CoroQueue
{
	Coroutine *head;
	Coroutine *tail;
	int size;
} CoroQueue;

CoroQueue *gqueue;

CoroQueue *createQueue( ) {
	CoroQueue *queue = (CoroQueue *)malloc( sizeof(CoroQueue) );
	queue->head = NULL;
	queue->tail = NULL;
	queue->size = 0;

	return queue;
}

void enqueue( CoroQueue *queue,  Coroutine *node) {
	if (queue -> size == 0) {
		queue-> head= node;
		queue-> tail = node;
	} else {
		queue->tail->next = node;
		queue->tail = node;
	}
	
	++queue -> size;
}

Coroutine *dequeue( CoroQueue *queue ) {
	if ( queue -> size == 0) {
		return NULL;
	} else  {
		Coroutine *tmp = queue->head;
		queue->head = tmp -> next;
		--queue -> size;

		return tmp;
	}
}

void scheduler( void ) {
	Coroutine *coroutine = dequeue(gqueue);
	sleep(1);
	printf("queuesize %d\n", gqueue->size);

	if (coroutine == NULL) {
		puts("exit main run");
		return;
	} else {
		gcurrent = coroutine;
		setcontext(gcurrent -> context);
	}
}


void init() {
	gqueue = createQueue();
	gmain = createCoroutine( scheduler , NULL, NULL);
	gcurrent = gmain;
}

void run() {
	setcontext(gcurrent -> context);
}


/*************
	test
**************/
struct H1 {
	int count;
};

void printH1( ) {
	puts("H1");
	int *i =  &(( struct H1 *)(gcurrent->env) )->count;
	
	while ( *i< 3) {
		printf("%s %d\n", "H1", *i);
		++*i ;

		enqueue(gqueue , gcurrent);
		swapcontext( gcurrent->context, gmain->context);
	}

	puts("after a while");
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

	/***
		variable storage for callback
	***/
	struct H1 *h1 = (struct H1 *)malloc( sizeof(struct H1));
	h1 -> count = 0;

	Coroutine *c1 = createCoroutine( printH1, (void *)h1, gmain );
	Coroutine *c2 = createCoroutine( printH2, NULL , gmain);

	enqueue( gqueue, c1);
	enqueue( gqueue, c2);


	run();
	return 0;
}