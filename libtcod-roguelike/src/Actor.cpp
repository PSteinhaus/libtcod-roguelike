#include "main.hpp"
#include <stdio.h>

Actor::Actor(int x, int y, int ch, const char* name, const TCODColor &col) : x(x), y(y), ch(ch), col(col), blocks(true),
attacker(NULL),destructible(NULL),ai(NULL),pickable(NULL),container(NULL) {
	strcpy(this->name, name);
}

Actor::~Actor() {
	if ( attacker ) delete attacker;
	if ( destructible ) delete destructible;
	if ( ai ) delete ai;
	if ( pickable ) delete pickable;
	if ( container ) delete container;
}

void Actor::update() {
	if ( ai ) ai->update(this);
}

/*
bool Actor::moveOrAttack(int x, int y) {
	if ( engine.map->isWall(x,y) ) return false;
	for ( Actor** iterator = engine.actors.begin(); iterator != engine.actors.end(); iterator++) {
		Actor* actor = *iterator;
		if ( actor->x == x && actor->y == y ) {
			printf("The %s laughs at your puny efforts to attack him!\n", actor->name);
			return false;
		}
	}
	this->x = x;
	this->y = y;
	return true;
}
*/

void Actor::render() const {
	TCODConsole::root->setChar(x,y,ch);
	TCODConsole::root->setCharForeground(x,y,col);
}