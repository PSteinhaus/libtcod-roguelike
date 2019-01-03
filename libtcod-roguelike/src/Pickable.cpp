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

LightningBolt::LightningBolt(float range, float damage)
   : range(range),damage(damage) {
}

bool LightningBolt::use(Actor* owner, Actor* user) {
	Actor* closestMonster = engine.getClosestMonster(true,user->x,user->y);
	if (!closestMonster) {
		engine.gui->message(TCODColor::lightGrey,"No enemy is close enough to strike.");
		return false;
	}
	// hit closest monster for <damage> hit points
	engine.gui->message(TCODColor::lightBlue,
		"A lightning bolt strikes the %s with a loud thunder!\nThe damage is %g hit points.", closestMonster->name, damage);
	closestMonster->destructible->takeDamage(closestMonster,damage);
	return Useable::use(owner,user);
}

Fireball::Fireball(float range, float damage, float areaRange) : LightningBolt(range,damage), areaRange(areaRange) {
}

bool Fireball::use(Actor* owner, Actor* user) {
	engine.gui->message(TCODColor::cyan, "Choose target,\n press Esc to cancel.");
	int x,y;
	if (! engine.pickATile(&x,&y,range)) {
		return false;
	}
	// burn everything in <range> (including player)
	engine.gui->message(TCODColor::orange,"The fireball explodes, burning everything within %g tiles!", areaRange);
	for (Actor **iterator=engine.actors.begin(); iterator != engine.actors.end(); iterator++) {
		Actor* actor = *iterator;
		if ( actor->destructible && !actor->destructible->isDead()
			&& actor->getDistance(x,y) <= areaRange)
		{
			engine.gui->message(TCODColor::orange,"The %s gets burned for %g hit points.",
			actor->name,damage);
			actor->destructible->takeDamage(actor,damage);
		}
	}
	return Useable::use(owner,user);
}

Confuser::Confuser(float range, int nbTurns) : range(range),nbTurns(nbTurns) {
}

bool Confuser::use(Actor* owner, Actor* user) {
	engine.gui->message(TCODColor::cyan, "Choose target,\n press Esc to cancel.");
	int x,y;
	if (!engine.pickATile(&x,&y,range)) return false;

	Actor* actor = engine.getActor(x,y);
	if (!actor || !actor->ai ) return false;

	// confuse the monster for <nbTurns> turns
	Ai* confusedAi = new ConfusedMonsterAi( nbTurns, actor->ai );
	actor->ai = confusedAi;
	engine.gui->message(TCODColor::lightGreen,"The eyes of the %s look vacant, as he starts to stumble around!", actor->name);
	return Useable::use(owner,user);
}