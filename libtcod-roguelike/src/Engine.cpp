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
				if ( i!=worldSize/2 || j!=worldSize/2 || k!=0 )
					world[i][j][k] = (k==0) ? new Chunk(Chunk::PLAINS) : new Chunk(Chunk::CAVE,false,k);
	world[worldSize/2][worldSize/2][0] = new Chunk(Chunk::WITCH_HUT,true);
}

void Engine::init() {
	player = new Actor(40,25,'@',"player",TCODColor::white);
	player->destructible = new PlayerDestructible(30,2,"your cadaver");
	player->attacker = new Attacker(5);
	player->ai = new PlayerAi();
	player->container = new Container(26,8);
	player->stomach = new Stomach(4000,2,80,0,6,4000);
	actors.push(player);
	map = new Map(80,43, currentChunk() );
	map->init();
	gui->message(TCODColor::red,
	"Welcome stranger!\nPrepare to perish in the Tombs of the Ancient Kings.");
	gameStatus = STARTUP;
}

void Engine::terminate() {
	actors.clearAndDelete();
	if ( map ) delete map;
	gui->clear();
}

Engine::~Engine() {
	terminate();
	delete gui;
	for(int i=0; i<worldSize; i++)
		for(int j=0; j<worldSize; j++)
			for(int k=0; k<worldDepth; k++)
				delete world[i][j][k];
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
		for (Actor** iterator=actors.begin(); iterator != actors.end(); iterator++) {
			Actor* actor=*iterator;
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
	for (Actor** iterator=actors.begin(); iterator != actors.end(); iterator++) {
		Actor* actor = *iterator;
		if ( actor != player && ((!actor->fovOnly && map->isExplored(actor->x,actor->y))
			|| map->isInFov(actor->x,actor->y)) ) {
			actor->render();
		}
	}
	player->render();
	// show the player's stats
	gui->render();
	/*
	TCODConsole::root->printf(1,screenHeight-2, "HP : %d/%d",
		(int)player->destructible->hp, (int)player->destructible->maxHp);
	*/
}

void Engine::sendToBack(Actor* actor) {
	actors.remove(actor);
	actors.insertBefore(actor,0);
}

Actor* Engine::getClosestMonster(bool fovRequired, int x, int y, float range) const {
	Actor* closest = NULL;
	float bestDistance = 1E6f;
	for (Actor** iterator=actors.begin();iterator != actors.end(); iterator++) {
		Actor* actor = *iterator;
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

Actor* Engine::getActor(int x, int y, bool aliveRequired) const {
	for (Actor** it = actors.begin(); it != actors.end(); it++) {
		Actor* actor = *it;
		if ( actor->x == x && actor->y == y && ( !aliveRequired ||
			(actor->destructible && !actor->destructible->isDead()) ) ) {
			return actor;
		}
	}
	return NULL;
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