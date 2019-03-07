#include "main.hpp"

HealthEffect::HealthEffect(float amount, const char *message) : amount(amount), message(message) {
}

void HealthEffect::applyTo(Actor *actor) {
	if (!actor->destructible) return;
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
}

ConfusionEffect::ConfusionEffect(int nbTurns, const char *message) : nbTurns(nbTurns), message(message) {
}

void ConfusionEffect::applyTo(Actor* actor) {
	if (!actor || !actor->ai ) return;

	// confuse the monster for <nbTurns> turns
	Ai* confusedAi = new ConfusedMonsterAi( nbTurns, actor->ai );
	actor->ai = confusedAi;
	if (message) engine.gui->message(TCODColor::lightGreen, message, actor->name);
}

DoorEffect::DoorEffect(char originalChar) : originalChar(originalChar) {
}

void DoorEffect::applyTo(Actor* actor) {
	if ( actor->blocks ) {
		actor->ch = '.';
		actor->switchBlocking();
	} else if ( engine.getActors(actor->x,actor->y, false).size()==1 ) { // if there is only the door and nothing else
		actor->ch = originalChar;
		actor->switchBlocking();
	}
}