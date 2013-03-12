#include <stdio.h>
#include <stdlib.h>
#include "../rr_math.h"
#include "../rr_array.h"

void arr_list(struct RRArray *arr)
{
        int *ptr = arr->ptr;
        printf("alloc: %zu\n", arr->nalloc);
        for(int i = 0; i < arr->nalloc; i++)
                printf(" %d", ptr[i]);
        printf("\nmemb: %zu\n", arr->nmemb);
        for(int i = 0; i < arr->nmemb; i++)
                printf(" %d", ptr[i]);
        printf("\n");
}


int main(int argc, char **argv)
{
        struct RRArray arr;
	rrarray_set(&arr, 10, sizeof(int));
        printf("nalloc: %zu nmemb: %zu\n", arr.nalloc, arr.nmemb);
        int *ptr = arr.ptr;
        for(int i = 0; i < arr.nmemb; i++)
                ptr[i] = i + 1;
        arr_list(&arr);
        rrarray_remove(&arr, 3);
        arr_list(&arr);
        rrarray_remove2(&arr, 3);
        arr_list(&arr);
        int v = 12;
        rrarray_push(&arr, &v);
        arr_list(&arr);
        rrarray_free(&arr);
        return 0;
}

