#include <SDL/SDL.h>
#include <stdbool.h>

Uint32
fullscreenflag(bool fullscreen) {
	return fullscreen ? SDL_FULLSCREEN : 0;
}
