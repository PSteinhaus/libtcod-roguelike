// helper functions that I didn't know where to put

#include "main.hpp"
#include <math.h>

bool numpadMove(int* x, int* y) {
	switch(engine.lastKey.vk) {
		case TCODK_KP8 :	// up
		case TCODK_UP :		*y += -1; return true;
		case TCODK_KP2 :	// down
		case TCODK_DOWN :	*y += 1; return true;
		case TCODK_KP4 :	// left
		case TCODK_LEFT :	*x += -1; return true;
		case TCODK_KP6 :	// right
		case TCODK_RIGHT :	*x += 1; return true;
		case TCODK_KP7 :	// up-left
							*x +=-1; *y +=-1; return true;
		case TCODK_KP9 :	// up-right
							*x +=1; *y +=-1; return true;
		case TCODK_KP1 :	// down-left
							*x +=-1; *y +=1; return true;
		case TCODK_KP3 :	// down-right
							*x +=1; *y +=1; return true;
		default: return false;
	}
}

float getDistance(int x0, int y0, int x, int y) {
	int dx = x0-x;
	int dy = y0-y;
	return sqrt(dx*dx + dy*dy);
}