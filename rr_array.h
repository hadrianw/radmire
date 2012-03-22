extern void rrarray_resize(struct RRArray *array, size_t nmemb);

extern size_t rrarray_push(struct RRArray *array, void *src);

// removes element at index
// moves last element to removed element
// returns index of moved element or/and size of resized array
extern size_t rrarray_remove(struct RRArray *array, size_t index);

extern size_t rrarray_remove2(struct RRArray *array, size_t index);

extern struct RRArray *rrarray(size_t nmemb, size_t size);

extern void rrarray_free(struct RRArray *array);
