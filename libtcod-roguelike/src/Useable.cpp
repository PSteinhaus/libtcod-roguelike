#include "main.hpp"

Useable::Useable(TargetSelector *selector, Effect *effect, bool destroyWhenEmpty) : selector(selector),effect(effect),destroyWhenEmpty(destroyWhenEmpty) {
}

Useable::~Useable() {
	if ( selector ) delete selector;
	if ( effect ) delete effect;
}

bool Useable::use(Actor* owner, Actor* user, bool implicitly) {
	if ( !effect->empty ) {
		// find targets
		TCODList<Actor *> list;
		TCODList<Tile *> tileList;
		if ( selector ) {
			selector->selectTargets(user, owner, list, tileList, implicitly);
		} else {
			list.push(user); // if no selector given just apply effect to the user
		}
		// apply effect to them
		bool succeed=false;
		for (Actor** it=list.begin(); it!=list.end(); it++) {
			if ( !effect->empty ) {
				if ( effect->applyTo(*it) )
					succeed=true;
			}
		}
		for (Tile** it=tileList.begin(); it!=tileList.end(); it++) {
			if ( !effect->empty ) {
				if ( effect->applyTo(*it) )
					succeed=true;
			}
		}
		// if the effect was used and is now empty then destroy yourself
		// (if you have to "destroyWhenEmpty")
		if ( succeed ) {
			if ( effect->empty && destroyWhenEmpty ) {
				if ( owner->onActorsList() ) engine.removeActor(owner,true);
				else if ( user->container ) {
					user->container->remove(owner);
					delete owner;
				}
			}
		}
		return succeed;
	} else return false;
}