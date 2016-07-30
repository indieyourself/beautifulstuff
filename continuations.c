/*
	gcc continuations.c -o ct
	https://spin.atomicobject.com/2012/11/01/hey-c-is-a-functional-language-too/

	uncomplete
*/
#include <stdbool.h>
#include <stdlib.h>
#include <stdio>

typedef struct IntList
{
	int value;
	IntList *next;
} Cons;

// continuation stuff
typedef void * const CPS_Result;

typedef void (*MakeListCallback)( Cons *list, CPS_Result result);

void make_list( int arraysize, int array[], Cons *newlist, MakeListCallback callback, CPS_Result result);

void reverse_and_stuff(Cons *list, CPS_Result result);

void stuff_list(Cons *list, CPS_Result result);

typedef void (*ReversedListCallback)( Cons *reversed_list, CPS_Result result);
void reverse( Cons *list, Cons *reversed_list, ReversedListCallback const callback, CPS_Result result);

typedef void(*VoidMappable)(int value);
void void_map_array( VoidMappable f, int size, int *array);
void print_value( int value);


int main() {

	int my_array[] = { 2, 5, 6, 1, 9, 23, 7654, 12, 0, 4};
   	int my_array_size = sizeof(my_array)/sizeof(my_array[0]);
  
  	int result[my_array_size];
  	/* call make_list and pass reverse_and_stuff as the "continuation". The action to perform next. */
  	make_list(my_array_size, my_array, NULL reverse_and_stuff, result);

  	/* print out our reversed array */
  	void_map_array(print_value, my_array_size, result);

	return 0;
}

/* constructs a linked list from an array */
void make_list( int array_size, int array[], Cons *new_list, MakeListCallback callback, CPS_Result result)
{
  if (array_size > 0)
  {
    Cons cell = { .value = array[array_size - 1], .next = new_list };
    make_list(array_size - 1, array, &cell, callback, result);
  }
  else
  {
    /* call our "continuation" with our result */
    callback(new_list, result);
  }
}

/* function that reverses a list and then stores it in an array */
void reverse_and_stuff(IntList list, CPS_Result result)
{
  reverse(list, Nil, stuff_list, result);
}

/* stuffs a linked list into an array */
void stuff_list(IntList list, CPS_Result result)
{
  if (Nil != list)
  {
    int32_t * array = result;
    array[0] = list->value;
    stuff_list(list->next, array + 1);
  }
}

void reverse( IntList list
            , IntList reversed_list
            , ReversedListCallback const callback
            , CPS_Result result)
{
  if (Nil == list)
  {
    callback(reversed_list, result);
  }
  else
  {
    /* build up the reversed list */
    Cons cell = { .value = list->value, .next = reversed_list };
    reverse(list->next, &cell, callback, result);
  }
}

/*
"loops" over an array and performs action f on each element
*/
void void_map_array( VoidMappable const f
                   , uint32_t const size
                   , int32_t const * const array)
{
  if (size > 0)
  {
    f(array[0]);
    void_map_array(f, size - 1, array + 1);
  }
}


void print_value(int32_t const value)
{
  printf("%d ", value);
}





