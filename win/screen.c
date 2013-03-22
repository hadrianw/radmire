#include <SDL/SDL_syswm.h>

int
setwindowed(int base) {
	SDL_SysWMinfo inf;
	SDL_VERSION(&inf.version);
	SDL_GetWMInfo(&inf);

	RECT r = {
		.right = screen.winsize.x,
		.bottom = screen.winsize.y
	};
	SetWindowLongPtr(inf.window, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
	AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, FALSE);
	screen.winsize = (Vec2i){r.right - r.left, r.bottom - r.top};
	r.left = (screen.fullsize.x - screen.winsize.x) / 2;
	r.top = (screen.fullsize.y - screen.winsize.y) / 2;
	MoveWindow(inf.window, r.left, r.top, screen.winsize.x, screen.winsize.y, TRUE);
	screen.size = &screen.winsize;

	return 0;
}

int
setfullscreen(int base) {
	SDL_SysWMinfo inf;
	SDL_VERSION(&inf.version);
	SDL_GetWMInfo(&inf);

	SetWindowLongPtr(inf.window, GWL_STYLE, WS_SYSMENU | WS_POPUP
	                 | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE);
	MoveWindow(inf.window, 0, 0, screen.fullsize.x, screen.fullsize.y, TRUE);
	screen.size = &screen.fullsize;

	return 0;
}
