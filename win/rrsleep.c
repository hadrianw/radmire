#include <SDL/SDL.h>
#include <time.h>

void rr_sleep(clock_t iv)
{
	SDL_Delay(iv * 1000 / CLOCKS_PER_SEC);
}
