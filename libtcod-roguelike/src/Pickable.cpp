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
			"You drop a %s.", owner->name);
		else engine.gui->message(TCODColor::lightGrey,"%s drops a %s.",
			carrier->name,owner->name);
	}
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
		if ( effect->applyTo(*it) )
			succeed=true;
	}
	if ( succeed && user->container ) {
		user->container->remove(owner);
		delete owner;
	}
	return succeed;
}