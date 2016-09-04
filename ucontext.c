/**
	http://man7.org/linux/man-pages/man3/getcontext.3.html
	http://www.gnu.org/software/libc/manual/html_mono/libc.html#System-V-contexts
	https://jianghong.wordpress.com/2008/10/04/coroutine-and-continuation-in-c/

	gcc -W ucontext.c -o ucontext
*/

#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>
#include <unistd.h>

ucontext_t gcontext;
ucontext_t gcontexts[3];
volatile int gstart = 0;

void printH1( ucontext_t *context) {
	getcontext(context);
	if (gstart == 0) { return; }
	puts("H1");

	if (gstart == 1) setcontext( &gcontext );
}

void printH2( ucontext_t *context ) {
	getcontext(context);
	if (gstart == 0) { return; }
	puts("H2");

	if (gstart == 1) setcontext( &gcontext );
}

void printH3( ucontext_t *context ) {
	getcontext(context);
	if (gstart == 0) { return; }
	puts("H3");

	if (gstart == 1) setcontext( &gcontext );
}

void whileSleep() {
	ucontext_t context;
	getcontext(&context);

	puts("Hello world");
	sleep(1);
	setcontext(&context);
}

void whileSleepFor() {
	ucontext_t context;
	int i = 0;

	//for ( ;i < 3; ++i)
	//{
	//	getcontext(&context);
	//	printf("i : %d\n", i);
	//}
	while(i < 3) {
		getcontext(&context);
		printf("i : %d\n", i);
		++i;
	}
	
	sleep(1);
	printf("sleep-i %d \n", i);
	setcontext(&context);
}


void rr(  int len) {
	int i = -1;
	gstart = 1;

	getcontext(&gcontext);

	++i;
	if ( i >= 3 ) { i = 0; }

	sleep(1);
	printf("%d\n", i);
	setcontext( &gcontexts[i]);
}

int main(void) {
	whileSleepFor();

	//printH1( &gcontexts[0] );
	//printH2( &gcontexts[1] );
	//printH3( &gcontexts[2] );


	rr(3);
	return 0;
}
