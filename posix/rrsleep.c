#include <sys/select.h>
#include <time.h>

void rr_sleep(clock_t iv)
{
	struct timeval tv = {
		.tv_usec = iv * 1000000 / CLOCKS_PER_SEC
	};
	select(0, 0, 0, 0, &tv);
}
