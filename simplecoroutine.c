/*
	gcc simplecoroutine.c -o sco
	http://stackoverflow.com/questions/14685406/practical-usage-of-setjmp-and-longjmp-in-c
*/
#include <stdio.h>
#include <setjmp.h>

jmp_buf bufA, bufB;

void routineB();
void routineA() {
	int r;

	printf("A(1)\n");

	r = setjmp(bufA);
	if (r == 0){
		routineB();
	}

	printf("A(2) r=%d\n", r);
} 

void routineB() {
	int r;

	printf("B(1)\n");

	r = setjmp(bufB);
	if (r == 0){
		longjmp(bufA, 110);
	}

	printf("B(2) r=%d\n", r);
}

int main(int argc, char const *argv[])
{
	routineA();
	return 0;
}
