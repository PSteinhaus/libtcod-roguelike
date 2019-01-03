#include "main.hpp"

Engine::Engine(int screenWidth, int screenHeight) : gameStatus(STARTUP), fovRadius(10),
	screenWidth(screenWidth), screenHeight(screenHeight)
{
	TCOD_console_set_custom_font("terminal16x16.png", 6, 16, 16);
	TCODConsole::initRoot(screenWidth, screenHeight, "libtcod C++ tutorial", false, TCOD_RENDERER_GLSL);
	player = new Actor(40,25,'@',"player",TCODColor::white);
	player->destructible = new PlayerDestructible(30,2,"your cadaver");
	player->attacker = new Attacker(5);
	player->ai = new PlayerAi();
	player->container = new Container(26);
	actors.push(player);
	map = new Map(80,43);
	gui = new Gui();
	gui->message(TCODColor::red,
	"Welcome stranger!\nPrepare to perish in the Tombs of the Ancient Kings.");
}

Engine::~Engine() {
	actors.clearAndDelete();
	delete map;
	delete gui;
}

void Engine::update() {
	if ( gameStatus == STARTUP ) map->computeFov();
	gameStatus=IDLE;

	TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &lastKey, NULL);
	player->update();
	if ( gameStatus == NEW_TURN ) {
		for (Actor** iterator=actors.begin(); iterator != actors.end(); iterator++) {
			Actor* actor=*iterator;
			if ( actor != player ) {
				actor->update();
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
		if ( map->isInFov(actor->x, actor->y) ) {
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
	cursorX = engine.player->x;	
	cursorY = engine.player->y;	
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
		// display the cursor
		TCODConsole::root->setChar(cursorX,cursorY, 'x');
		// react to input (move the cursor or accept)
		TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &lastKey, NULL);
	}
	return false;
}