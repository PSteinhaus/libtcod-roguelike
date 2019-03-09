#include "main.hpp"

HealthEffect::HealthEffect(float amount, const char *message) : amount(amount), message(message) {
}

bool HealthEffect::applyTo(Actor *actor) {
	if (!actor->destructible) return false;
	if ( amount > 0 ) {
		float pointsHealed=actor->destructible->heal(amount);
		if (pointsHealed > 0) {
			if ( message ) {
				engine.gui->message(TCODColor::lightGrey,message,actor->name,pointsHealed);
			}
		}
	} else {
		float damage = actor->destructible->calcDamage(-amount);
		if ( message && damage > 0 ) {
			engine.gui->message(TCODColor::lightGrey,message,actor->name,damage);
		}
		actor->destructible->takeDamage(actor,damage);
	}
	return true;
}

ConfusionEffect::ConfusionEffect(int nbTurns, const char *message) : nbTurns(nbTurns), message(message) {
}

bool ConfusionEffect::applyTo(Actor* actor) {
	if (!actor || !actor->ai ) return false;

	// confuse the monster for <nbTurns> turns
	Ai* confusedAi = new ConfusedMonsterAi( nbTurns, actor->ai );
	actor->ai = confusedAi;
	if (message) engine.gui->message(TCODColor::lightGreen, message, actor->name);
	return true;
}

DoorEffect::DoorEffect(char originalChar) : originalChar(originalChar) {
}

bool DoorEffect::applyTo(Actor* actor) {
	if ( actor->blocks ) {
		actor->ch = '.';
		actor->switchBlocking();
		return true;
	} else if ( engine.getActors(actor->x,actor->y, false).size()==1 ) { // if there is only the door and nothing else (is probably gonna change as soon as really small actors start being around)
		actor->ch = originalChar;
		actor->switchBlocking();
		return true;
	}
	return false;
}