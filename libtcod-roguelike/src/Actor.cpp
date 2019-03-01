#include "main.hpp"
#include <stdio.h>
#include <math.h>

Actor::Actor(int x, int y, int ch, const char* name, const TCODColor &col, float volume, float weight) : x(x), y(y), ch(ch), col(col), blocks(true), fovOnly(true),
attacker(NULL),destructible(NULL),ai(NULL),pickable(NULL),container(NULL),stomach(NULL),interactable(NULL), volume(volume),weight(weight),actorRepName(ActorRep::NONE) {
	strcpy(this->name, name);
}

Actor::~Actor() {
	if ( attacker ) delete attacker;
	if ( destructible ) delete destructible;
	if ( ai ) delete ai;
	if ( pickable ) delete pickable;
	if ( container ) delete container;
	if ( stomach ) delete stomach;
}

void Actor::update() {
	if ( ai ) ai->update(this);
}

void Actor::render() const {
	TCODConsole::root->setChar(x,y,ch);
	TCODConsole::root->setCharForeground(x,y,col);
}

float Actor::getDistance(int cx, int cy) const {
	int dx = x-cx;
	int dy = y-cy;
	return sqrt(dx*dx + dy*dy);
}

float Actor::getVolume() {
	float totalVolume = volume;
	if ( container )	totalVolume += container->getVolume();
	if ( stomach )		totalVolume += stomach->getVolume();
	return totalVolume;
}