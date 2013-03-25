#include <SDL/SDL_syswm.h>
#include <stdbool.h>

typedef struct {
	LONG_PTR attrs;
	int x, y;
} Mode;

static bool prev;
static Mode full = { WS_SYSMENU | WS_POPUP | WS_VISIBLE
                     | WS_CLIPCHILDREN | WS_CLIPSIBLINGS };
static Mode win = { WS_OVERLAPPEDWINDOW | WS_VISIBLE };

Uint32
fullscreenflag(bool fullscreen) {
	SDL_SysWMinfo inf;

	SDL_VERSION(&inf.version);
	if(SDL_GetWMInfo(&inf) != 1)
		return 0;

	if(!prev && fullscreen) {
		RECT r;
		GetWindowRect(inf.window, &r);
		win.x = r.left;
		win.y = r.top;
	}

	Mode *mode = fullscreen ? &full : &win;
	SetWindowLongPtr(inf.window, GWL_STYLE, mode->attrs);
	if(prev != fullscreen)
		MoveWindow(inf.window, mode->x, mode->y, 0, 0, FALSE);

	prev = fullscreen;
	return 0;
}
