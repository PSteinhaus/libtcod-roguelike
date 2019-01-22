#include "main.hpp"

Pickable::Pickable() : useable(NULL) {
}

Pickable::~Pickable() {
	if ( useable ) delete useable;
}

bool Pickable::pick(Actor* owner, Actor* picker) {
	if ( picker->container && picker->container->add(owner) ) {
		engine.actors.remove(owner);
		return true;
	}
	return false;
}

bool Pickable::use(Actor* owner, Actor* user) {
	if ( user->container && useable ) {
		return useable->use(owner, user);
	}
	engine.gui->message(TCODColor::white, "%s cannot be used", owner->name);
	return false;
}

void Pickable::drop(Actor* owner, Actor* carrier) {
	if ( carrier->container ) {
		carrier->container->remove(owner);
		engine.actors.insertBefore(owner, 0);
		owner->x = carrier->x;
		owner->y = carrier->y;
		if (carrier == engine.player) engine.gui->message(TCODColor::lightGrey,
			"You drop the %s.", owner->name);
		else engine.gui->message(TCODColor::lightGrey,"%s drops a %s.",
			carrier->name,owner->name);
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

Useable::Useable(TargetSelector *selector, Effect *effect) : selector(selector), effect(effect) {
}

Useable::~Useable() {
	if ( selector ) delete selector;
	if ( effect ) delete effect;
}

bool Useable::use(Actor* owner, Actor* user) {
	TCODList<Actor *> list;
	if ( selector ) {
		selector->selectTargets(user, list);
	} else {
		list.push(user);
	}
	
	bool succeed=false;
	for (Actor **it=list.begin(); it!=list.end(); it++) {
		effect->applyTo(*it);
		succeed=true;
	}
	if ( succeed && user->container ) {
		user->container->remove(owner);
		delete owner;
	}
	return succeed;
}