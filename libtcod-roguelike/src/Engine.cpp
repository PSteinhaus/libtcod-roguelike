#include "main.hpp"

Engine::Engine(int screenWidth, int screenHeight) : player(NULL), map(NULL), gameStatus(STARTUP), fovRadius(10),
	screenWidth(screenWidth), screenHeight(screenHeight), depth(1),x(worldSize/2),y(worldSize/2)
{
	TCODSystem::setFps(60);
	TCOD_console_set_custom_font("terminal16x16.png", 6, 16, 16);
	TCODConsole::initRoot(screenWidth, screenHeight, "witchRogue prototype", false, TCOD_RENDERER_GLSL);
	gui = new Gui();
	for(int i=0; i<worldSize; i++)
		for(int j=0; j<worldSize; j++)
			for(int k=0; k<worldDepth; k++)
				world[i][j][k] = new Chunk();
}

void Engine::init() {
	// generate the world
	for(int i=0; i<worldSize; i++)
		for(int j=0; j<worldSize; j++)
			for(int k=0; k<worldDepth; k++) {
				if ( i!=worldSize/2 || j!=worldSize/2 || k!=0 )
					world[i][j][k] = (k==0) ? new Chunk(Chunk::PLAINS) : new Chunk(Chunk::CAVE,false,k);
			}
	world[worldSize/2][worldSize/2][0] = new Chunk(Chunk::WITCH_HUT,true);

	depth = 1;
	x = worldSize/2;
	y = worldSize/2;
	map = new Map(80,43, currentChunk() );
	player = new Actor(40,25,'@',"player",TCODColor::white);
	player->destructible = new PlayerDestructible(30,2,"your cadaver");
	player->attacker = new Attacker(5);
	player->ai = new PlayerAi();
	player->container = new Container(26,8);
	player->stomach = new Stomach(4000,2,80,0,6,4000);
	addActor(player);
	map->init();
	gui->message(TCODColor::red,"Welcome stranger.");
	gameStatus = STARTUP;
}

void Engine::terminate() {
	for (auto it=actorsBegin(); it!=actorsEnd(); it++)
		delete *it;
	actors.clear();
	actorsAt.clear();
	if ( map ) delete map;
	gui->clear();
	// delete world
	for(int i=0; i<worldSize; i++)
		for(int j=0; j<worldSize; j++)
			for(int k=0; k<worldDepth; k++)
				delete world[i][j][k];
}

Engine::~Engine() {
	terminate();
	delete gui;
}

void Engine::update() {
	if ( gameStatus == STARTUP ) map->computeFov();
	gameStatus=IDLE;

	TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &lastKey, NULL);
	if (lastKey.vk == TCODK_ESCAPE) {
		gameMenu();
	}
	player->update();
	if ( gameStatus == NEW_TURN ) {
		player->stomach->digest(player);
		for (auto it=actorsBegin(); it!=actorsEnd(); it++) {
			Actor* actor=*it;
			if ( actor != player ) {
				actor->update();
				if ( actor->stomach ) actor->stomach->digest(actor);				
			}
		}
	}
}

void Engine::render() {
	TCODConsole::root->clear();
	// draw the map
	map->render();
	// draw the actors
	enum DrawState { STRUCTURES, ITEMS, CREATURES, DONE } state = STRUCTURES;
	while ( state != DONE ) {
		for (auto it=actorsBegin(); it!=actorsEnd(); it++) {
			Actor* actor = *it;
			switch(state) {
				case STRUCTURES:
					// draw dead non-items first
					if ( actor->destructible || actor->pickable ) continue;
				break;
				case ITEMS:
					// draw items
					if ( !actor->pickable ) continue;
				break;
				case CREATURES:
					// draw the rest
					if ( !actor->destructible || actor->pickable ) continue;
				break;
				default:;
			}
			// if in sight (or !fovOnly)
			if ( actor != player && map->inMap(actor->x,actor->y) && ((!actor->fovOnly && map->isExplored(actor->x,actor->y))
				|| map->isInFov(actor->x,actor->y)) )
			{
				actor->render();
			}
		}
		state = (DrawState)( ((int)state)+1 ); //state++
	}

	player->render();
	// show the player's stats
	gui->render();
}

void Engine::sendToBack(Actor* actor) {
	Point position = Point(actor->x, actor->y);
	actorsAt[position].remove(actor);
	actorsAt[position].insertBefore(actor,0);
}

std::list<Actor*>::iterator Engine::actorsBegin() {
	return actors.begin();
}

std::list<Actor*>::iterator Engine::actorsEnd() {
	return actors.end();
}

void Engine::moveActor(Actor* actor, int x, int y) {
	removeActorFromPos(actor);	// remove from old position
	addActorToPos(actor, x,y);		// place at new position
}

void Engine::removeActorFromPos(Actor* actor) {
	// remove the actor from actorsAt
	actorsAt[Point(actor->x, actor->y)].remove(actor);
	// remove the actor from the map
	map->removeActor(actor);
}

std::list<Actor*>::iterator Engine::removeActor(std::list<Actor*>::iterator iterator, bool destroy) {
	Actor* actor = *iterator;
	// remove the actor from actorsAt
	removeActorFromPos(actor);
	// remove from actors
	iterator = actors.erase(iterator);
	iterator--;
	// if the actor isn't supposed to be stored elsewhere it needs to be deleted
	if (destroy) delete actor;
	return iterator; // returns an iterator that, when incremented, delivers the next actor in line
}

void Engine::removeActor(Actor* actor, bool destroy) {
	// remove the actor from actorsAt
	removeActorFromPos(actor);
	// remove from actors
	actors.remove(actor);
	// if the actor isn't supposed to be stored elsewhere it needs to be deleted
	if (destroy) delete actor;
}

void Engine::addActorToPos(Actor* actor, int x, int y) {
	// (default is -1), so the default is to place the actor at its (internal) position
	// if not, then place the actor at the specified position
	if (x == -1) x = actor->x; else actor->x = x;
	if (y == -1) y = actor->y; else actor->y = y;
	// add it at the new position
	actorsAt[Point(x,y)].push(actor);
	// add the actor to the map
	map->addActor(actor);
}

void Engine::addActor(Actor* actor, int x, int y) {
	actors.push_back(actor);
	addActorToPos(actor,x,y);
}

int Engine::totalActors() const {
	return actors.size();
}

Tile* Engine::tileAt(int x, int y) const {
	return map->tileAt(x,y);
}

void Engine::entitiesInRange( int x0, int y0, float range, bool aliveRequired, TCODList<Actor*>* list, TCODList<Tile*>* tileList ) {
	for ( int i=(int)-range; i<=(int)range; ++i )
		for ( int j=(int)-range; j<=(int)range; ++j ) {
			int x = x0 + i;
			int y = y0 + j;
			if(!map->inMap(x,y)) continue;
			if ( getDistance(x0,y0, x,y) <= range ) {
				if(list) list->addAll( getActors(x,y,aliveRequired) );
				if(tileList) tileList->push( tileAt(x,y) );
			}
		}
}

Actor* Engine::getClosestMonster(bool fovRequired, int x, int y, float range) {
	Actor* closest = NULL;
	float bestDistance = 1E6f;
	for (auto it=actorsBegin(); it!=actorsEnd(); it++) {
		Actor* actor = *it;
		if ( actor != player && actor->destructible && !actor->destructible->isDead() ) {
			float distance = actor->getDistance(x,y);
			if ( distance < bestDistance && ( distance <= range || range == 0.0f) &&
				( !fovRequired || map->isInFov(actor->x,actor->y) ) ) {
				bestDistance = distance;
				closest = actor;
			}
		}
	}
	return closest;
}

bool Engine::pickATile(int* x, int* y, float maxRange) {
	int cursorX, cursorY;
	cursorX = player->x;	
	cursorY = player->y;	
	while ( !TCODConsole::isWindowClosed() ) {
		render();
		// highlight the possible range
		for (int cx = 0; cx < map->width; cx++)
			for (int cy = 0; cy < map->height; cy++)
				if ( map->isInFov(cx,cy) && (maxRange==0 || player->getDistance(cx,cy) <= maxRange) ) {
					TCODColor col = TCODConsole::root->getCharBackground(cx,cy);
					col = col * 1.2f;
					if (col == TCODColor::black) col = TCODColor::darkerGrey;
					TCODConsole::root->setCharBackground(cx,cy,col);
				}
		// check for collision and draw a line
		int lineX = player->x;
		int lineY = player->y;
		bool collision = false;
		TCODLine::init(lineX,lineY, cursorX,cursorY);
		do {
			// draw the line
			TCODColor col = TCODConsole::root->getCharBackground(lineX,lineY);
			col = col * 2.0f;
			TCODConsole::root->setCharBackground(lineX,lineY,col);
			// check for collision
			if ( map->isWall(lineX,lineY) ) { collision = true; break; }
		} while ( !TCODLine::step(&lineX,&lineY) );
		// display the cursor
		TCODColor front = TCODColor::lightRed;
		TCODColor back  = TCODColor::darkerRed;
		if ( !collision && player->getDistance(cursorX,cursorY) <= maxRange ) {
			front = TCODColor::lightGreen;
			back = TCODColor::darkerGreen;
		}
		TCODConsole::root->putCharEx(cursorX,cursorY,'x',front,back);

		TCODConsole::flush();

		// react to input (move the cursor, accept or escape)
		TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &lastKey, NULL);
		numpadMove(&cursorX, &cursorY);
		switch(lastKey.vk) {
			case TCODK_ENTER :	*x = lineX;
								*y = lineY;
								return true;
			case TCODK_ESCAPE :	return false;
			default: break;
		}
	}
	return false;
}

Actor* Engine::getActor(int x, int y, bool aliveRequired) {
	/*
	for (Actor** it = actors.begin(); it != actors.end(); it++) {
		Actor* actor = *it;
		if ( actor->x == x && actor->y == y && ( !aliveRequired ||
			(actor->destructible && !actor->destructible->isDead()) ) ) {
			return actor;
		}
	}
	return NULL;*/

	TCODList<Actor*>& list = actorsAt[Point(x,y)];
	for (Actor** it = list.begin(); it != list.end(); it++) {
		Actor* actor = *it;
		if ( !aliveRequired || (actor->destructible && !actor->destructible->isDead()) ) {
			return actor;
		}
	}
	return NULL;


}

TCODList<Actor*> Engine::getActors(int x, int y, bool aliveRequired) {
	/*
	TCODList<Actor*> list;
	for (Actor** it = actors.begin(); it != actors.end(); it++) {
		Actor* actor = *it;
		if ( actor->x == x && actor->y == y && ( !aliveRequired ||
			(actor->destructible && !actor->destructible->isDead()) ) ) {
			list.push(actor);
		}
	}
	return list;*/

	TCODList<Actor*> list = actorsAt[Point(x,y)];
	if (aliveRequired) {
		for (Actor** it = list.begin(); it != list.end(); it++) {
			Actor* actor = *it;
			if ( !actor->destructible || actor->destructible->isDead() ) {
				it = list.remove(it);
			}
		}
	}
	return list;
}

void Engine::changeChunk(int dx, int dy, int dz) {
	map->leave(); // deletes the Map (and offloads units)

	if ( dz == 1 )
		gui->message(TCODColor::red,"You descend\ndeeper into the heart of the dungeon...");

	depth += dz;
	x += dx;
	y += dy;

	Chunk* curCh = currentChunk();
	if ( !curCh->map ) { // if this chunk has no (persistent) map
		// create a new map
		map = new Map(80,43, curCh );
		map->init();
	} else {
		map = curCh->map;
		map->loadSavedActors();
	}
	gameStatus = STARTUP;

	// try to place stairs at the players position (for logical reasons)
	/*
	if ( dz != 0 ) {
		for (Actor** it = engine.actors.begin(); it != engine.actors.end(); it++) {
			if ( (dz == 1  && (*it)->ch == '<') ||
				 (dz == -1 && (*it)->ch == '>') )
			{
				// if the new map is persistent then the player has to be placed
				// at the stairs (instead of the other way around)
				if ( curCh->persistentMap ) {
					player->x = (*it)->x;
					player->y = (*it)->y;
				} else {
					(*it)->x = player->x;
					(*it)->y = player->y;
				}
				break;
			}
		}
	}*/
	if ( dz != 0 ) {
		for (auto it=actorsBegin(); it!=actorsEnd(); it++) {
			Actor* actor = *it;
			if ( (dz == 1  && actor->ch == '<') ||
				 (dz == -1 && actor->ch == '>') )
			{
				// if the new map is persistent then the player has to be placed
				// at the stairs (instead of the other way around)
				if ( curCh->persistentMap ) {
					moveActor(player, actor->x, actor->y);
				} else {
					moveActor(actor, player->x, player->y);
				}
				break;
			}
		}
	}

}

void Engine::gameMenu() {
	gui->menu.clear();
	gui->menu.addItem(Menu::NEW_GAME,"New game");
	if ( TCODSystem::fileExists("game.sav")) {
		gui->menu.addItem(Menu::CONTINUE,"Continue");
	}
	gui->menu.addItem(Menu::EXIT,"Exit");

	Menu::MenuItemCode menuItem=engine.gui->menu.pick();
	if ( menuItem == Menu::EXIT || menuItem == Menu::NONE ) {
		// Exit or window closed
		gameStatus = EXIT;
	} else if ( menuItem == Menu::NEW_GAME ) {
		// New game
		terminate();
		init();
	} else if ( !map ) {
		load();
	}
}

bool Engine::waitForDirection(bool acceptCenter){
	int x=0;
	int y=0;
	while ( !numpadMove(&x,&y) || (x==0 && y==0 && !acceptCenter) ) {
		TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &lastKey, NULL);
		if (lastKey.vk == TCODK_ESCAPE) return false;
	}
	return true;
}
