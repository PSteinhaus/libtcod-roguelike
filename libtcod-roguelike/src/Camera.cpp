#include "main.hpp"

Camera::Camera(int x, int y, int width, int height) : x(x),y(y),width(width),height(height), mapX(0),mapY(0) {
	con = new TCODConsole(width, height);
}

Camera::~Camera() {
	delete con;
}

void Camera::setToPos(int x, int y) {
	mapX = x;
	mapY = y;
}

bool Camera::isOnCamera(int x0, int y0) const {
	return ( x0>=mapX && x0<mapX+width && y0>=mapY && y0<mapY+height );
}


void Camera::render() {
	// clear the console
	con->setDefaultBackground(TCODColor::black);
	con->clear();
	// draw the map
	engine.map->render(con, mapX,mapY);
	// draw the actors
	enum DrawState { STRUCTURES, ITEMS, CREATURES, DONE } state = STRUCTURES;
	while ( state != DONE ) {
		for (auto it=engine.actorsBegin(); it!=engine.actorsEnd(); it++) {
			Actor* actor = *it;
			if ( !isOnCamera(actor->x,actor->y) ) continue;
			switch(state) {
				case STRUCTURES:
					// draw dead non-items first
					if ( actor->pickable || (actor->destructible && !actor->destructible->isDead() ) ) continue;
				break;
				case ITEMS:
					// draw items
					if ( !actor->pickable ) continue;
				break;
				case CREATURES:
					// draw the (living) rest
					if ( !actor->destructible || actor->destructible->isDead() ) continue;
				break;
				default:;
			}
			// if in sight (or !fovOnly)
			if ( actor != engine.player && engine.map->inMap(actor->x,actor->y) && ((!actor->fovOnly && engine.map->isExplored(actor->x,actor->y))
				|| engine.map->isInFov(actor->x,actor->y)) )
			{
				actor->render(con, mapX,mapY);
			}
		}
		state = (DrawState)( ((int)state)+1 ); //state++
	}
	if ( isOnCamera(engine.player->x,engine.player->y) )
		engine.player->render(con, mapX,mapY);
	// blit the console onto the root console
	TCODConsole::blit(con,0,0,0,0, TCODConsole::root,x,y);
}
