#include "main.hpp"
#include <stdio.h>
#include <math.h>

Actor::Actor(int x, int y, int ch, const char* name, const TCODColor &col, float volume, float weight) : x(x), y(y), ch(ch), col(col), blocks(true), transparent(true), fovOnly(true),
attacker(NULL),destructible(NULL),ai(NULL),pickable(NULL),container(NULL),stomach(NULL),interactable(NULL),body(NULL), volume(volume),weight(weight),actorRepName(ActorRep::NONE) {
	strcpy(this->name, name);
}

Actor::~Actor() {
	if ( attacker ) delete attacker;
	if ( destructible ) delete destructible;
	if ( ai ) delete ai;
	if ( pickable ) delete pickable;
	if ( container ) delete container;
	if ( stomach ) delete stomach;
	if ( interactable ) delete interactable;
	if ( body ) delete body;
}

void Actor::update() {
	if ( ai ) ai->update(this);
}

void Actor::render(TCODConsole* con, int xOffset, int yOffset) const {
	const int renderX = x-xOffset;
	const int renderY = y-yOffset;
	con->setChar(renderX,renderY,ch);
	con->setCharForeground(renderX,renderY,col);
}

float Actor::getDistance(int cx, int cy) const {
	int dx = x-cx;
	int dy = y-cy;
	return sqrt(dx*dx + dy*dy);
}

float Actor::getVolume() const {
	float totalVolume = volume;
	if ( container )	totalVolume += container->getVolume();
	if ( stomach )		totalVolume += stomach->getVolume();
	return totalVolume;
}

bool Actor::onActorsList() const {
	for ( auto it=engine.actorsBegin(); it!=engine.actorsEnd(); ++it )
		if ( *it == this ) return true;
	return false;
}


void Actor::switchBlocking() {
	blocks = !blocks;
	engine.map->computeTCODMapAt(x,y);
}

void Actor::switchTransparent() {
	transparent = !transparent;
	engine.map->computeTCODMapAt(x,y);
	engine.map->computeFov();
}

bool Actor::store(Actor* item) {
	if ( container ) return container->add(item);
	return false;
}

bool Actor::equip(Actor* equipment) {
	if ( body ) {
		Actor* oldEquipment = body->equip(this, equipment);	// try to equip
		if ( oldEquipment==equipment ) return false;		// equipment was rejected
															// else: success!
		if ( oldEquipment!=NULL ) {							// if something was unequiped
			if (!store(oldEquipment))						// try to store it (in your inventory)
				engine.addActor(oldEquipment, x,y);			// and drop it if you can't
		}
		return true;
	}
	return false;
}

bool Actor::unequip(Actor* equipment) {
	if ( body ) {
		if ( body->unequip(this, equipment) ) {				// if you successfully unequip it
			if (!store(equipment))						// try to store it (in your inventory)
				engine.addActor(equipment, x,y);			// and drop it if you can't
			return true;		
		}
	}
	return false;
}

bool Actor::equipableOn(Actor* actor, BodyPart::EquipmentSlot slot) {
	if( pickable && pickable->equipable )
		return pickable->equipable->equipableOn(actor,slot);
	return false;
}

void Actor::equipOn(Actor* actor) {
	if( pickable && pickable->equipable )
		pickable->equipable->equipOn(actor);
}

void Actor::unequipOn(Actor* actor) {
	if( pickable && pickable->equipable )
		pickable->equipable->unequipOn(actor);
}