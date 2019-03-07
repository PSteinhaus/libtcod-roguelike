#include "main.hpp"
#include <stdio.h>

Attacker::Attacker(float power) : power(power) {
}

void Attacker::attack(Actor* owner, Actor* target) {
	if ( target->destructible && ! target->destructible->isDead() ) {
		float damage = target->destructible->calcDamage(power);
		if ( damage > 0 ) {
			engine.gui->message(TCODColor::white,
			"%s attacks %s for %g hit points", owner->name, target->name, damage );
		} else {
			engine.gui->message(TCODColor::white,
			"%s attacks %s, but it has no effect!", owner->name, target->name);
		}
		target->destructible->takeDamage(target, damage);
	} else {
		engine.gui->message(TCODColor::white,
		"%s attacks %s in vain.", owner->name, target->name);
	}
}