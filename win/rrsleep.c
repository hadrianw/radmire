#include <SDL/SDL.h>
#include <time.h>

void rr_sleep(clock_t iv)
{
	SDL_Delay(tv * 1000 / CLOCKS_PER_SEC);
}
