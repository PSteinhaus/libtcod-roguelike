#include "main.hpp"

Pickable::Pickable() : useable(NULL) {
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

bool Useable::use(Actor* owner, Actor* user) {
	if ( user->container ) {
		user->container->remove(owner);
		delete owner;
		return true;
	}
	return false;
}

// USEABLES

Healer::Healer(float amount) : amount(amount) {
}

bool Healer::use(Actor* owner, Actor* user) {
	if ( user->destructible ) {
		float amountHealed = user->destructible->heal(amount);
		if ( amountHealed > 0 ) {
			return Useable::use(owner,user);
		}
	}
	return false;
}