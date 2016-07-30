/*	
	gcc exception.c -o ex

	https://zh.wikipedia.org/wiki/Setjmp.h
	http://www.di.unipi.it/~nids/docs/longjump_try_trow_catch.html
*/

#include <stdio.h>
#include <setjmp.h>

static jmp_buf buf;

void first(void) {
	printf("Thorw Now From first\n");
	longjmp(buf, 1);
	printf("End first\n");
}

int main(int argc, char const *argv[]) {
	if ( !setjmp(buf) ) {
		printf("In Try Block Now\n");
		first();
	} else {
		printf("In Exception Handle Block Now\n");
	}
	return 0;
}
