#include <stdlib.h>
#include <string.h>
#include "rr_math.h"
#include "rr_types.h"

void rrarray_resize(struct RRArray *array, size_t nmemb)
{
        size_t nalloc = to_pow2(nmemb);
        if(nmemb < array->nmemb && array->nalloc > nalloc * 2)
                nalloc *= 2;
        array->ptr = realloc(array->ptr, nalloc * array->size);
        array->nalloc = nalloc;
        array->nmemb = nmemb;
}

size_t rrarray_push(struct RRArray *array, void *src)
{
        size_t last = array->nmemb * array->size;
        rrarray_resize(array, array->nmemb + 1);
        memcpy((char*)array->ptr + last, src, array->size);
        return last;
}

// removes element at index
// moves last element to removed element
// returns index of moved element or/and size of resized array
size_t rrarray_remove(struct RRArray *array, size_t index)
{
        size_t last = array->nmemb - 1;
        if(last != index) {
                char *ptr = array->ptr;
                index *= array->size;
                last *= array->size;
                memcpy(ptr + index, ptr + last, array->size);
        }
        rrarray_resize(array, array->nmemb - 1);
        return array->nmemb;
}

size_t rrarray_remove2(struct RRArray *array, size_t index)
{
        char *ptr = array->ptr;
        ptr += index * array->size;
        memmove(ptr, ptr + array->size, (array->nmemb - index - 1) * array->size);
        rrarray_resize(array, array->nmemb - 1);
        return array->nmemb;
}

struct RRArray *rrarray(size_t nmemb, size_t size)
{
        struct RRArray *array = calloc(nmemb, size);
        array->size = size;
        rrarray_resize(array, nmemb);
        return array;
}

void rrarray_free(struct RRArray *array)
{
        free(array->ptr);
        free(array);
}

