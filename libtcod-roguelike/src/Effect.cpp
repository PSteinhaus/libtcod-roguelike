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
			return true;
		}
	} else {
		if ( message && -amount-actor->destructible->defense > 0 ) {
			engine.gui->message(TCODColor::lightGrey,message,actor->name,-amount-actor->destructible->defense);
		}
		if ( actor->destructible->takeDamage(actor,-amount) > 0 ) {
			return true;
		}
	}
	return false;
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