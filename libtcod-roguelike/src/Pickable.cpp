#include "main.hpp"

Pickable::Pickable() : useable(NULL), equipable(NULL) {
	digestor = NULL;
}

Pickable::~Pickable() {
	if ( useable ) delete useable;
	if ( digestor ) delete digestor;
	if ( equipable ) delete equipable;
}

bool Pickable::pick(Actor* owner, Actor* picker) {
	if ( picker->container && picker->container->add(owner) ) {
		engine.removeActor(owner, false);
		return true;
	}
	return false;
}

bool Pickable::use(Actor* owner, Actor* user) {
	if ( useable ) {
		return useable->use(owner, user);
	}
	engine.gui->message(TCODColor::white, "%s cannot be used", owner->name);
	return false;
}

void Pickable::drop(Actor* owner, Actor* carrier) {
	if ( carrier->container ) {
		carrier->container->remove(owner);
		engine.addActor(owner, carrier->x, carrier->y);
		engine.sendToBack(owner);
		if (carrier == engine.player) engine.gui->message(TCODColor::lightGrey,
			"You drop the %s.", owner->name);
		else engine.gui->message(TCODColor::lightGrey,"%s drops a %s.",
			carrier->name,owner->name);
	}
}

void Pickable::vomit(Actor* owner, Actor* carrier) {
	if ( carrier->stomach ) {
		carrier->stomach->remove(owner);
		engine.addActor(owner, carrier->x, carrier->y);
		engine.sendToBack(owner);
		owner->col = TCODColor::lightGreen;
		if (carrier == engine.player) engine.gui->message(TCODColor::lightGreen, "You vomit.");
		else engine.gui->message(TCODColor::lightGreen,"The %s vomits.", carrier->name);
	}
}

bool Pickable::eat(Actor* owner, Actor* eater) {
	if ( eater->stomach->add(owner) ) {
		if ( eater->container ) {
			eater->container->remove(owner);
		}
		if (eater == engine.player) engine.gui->message(TCODColor::lightGrey,
			"You eat the %s.", owner->name);
		else engine.gui->message(TCODColor::lightGrey,"%s eats a %s.",
			eater->name,owner->name);
		return true;
	}
	// you cannot eat the item
	else if (eater == engine.player) engine.gui->message(TCODColor::lightGrey,
		"Eating the %s would be too much for you.", owner->name);
	return false;
}